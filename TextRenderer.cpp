#include "TextRenderer.h"
#include <iostream>
#include <sstream>

using namespace std;


TextRenderer::TextRenderer()
{
    MagickWandGenesis();    // just once somewhere
    mw = NewMagickWand();

    MagickSetSize(mw,1024,64);
    MagickSetPointsize(mw,48);
    MagickSetFont(mw,"Sans");
    MagickSetOption(mw,"colorspace","GRAY");    // does this do anything?
    MagickSetOption(mw,"fill","white");
    MagickSetOption(mw,"background","rgba(0,0,0,0)");   // transparent background
    MagickSetOption(mw,"encoding","unicode");   // does this do anything?
    MagickSetGravity(mw,CenterGravity);
}

TextRenderer::~TextRenderer()
{
    if(mw)DestroyMagickWand(mw);
}

void TextRenderer::test(const string &text)
{
    MagickReadImage(mw, text.c_str());
    MagickWriteImage(mw,"caption.gif");
    MagickReadImage(mw, "label:A");
    MagickWriteImage(mw,"caption2.gif");
}

void TextRenderer::Text2Texture(const GLuint texhandle, const string &text)
{
    _watch.Start();

    MagickReadImage(mw, text.c_str());

    unsigned char *Buffer = NULL;
    size_t width = MagickGetImageWidth(mw);
    size_t height = MagickGetImageHeight(mw);
    Buffer = new unsigned char[width * height];

    cout << "w="<<width << " h="<<height << endl;

    // Export the whole image
    MagickExportImagePixels(mw, 0, 0, width, height, "I", CharPixel, Buffer);

    glBindTexture(GL_TEXTURE_2D, texhandle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, Buffer);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    delete [] Buffer;

    _watch.Stop();

    cout << "Text generation took: " << _watch.TimeElapsed() << " s" << endl;

}


void Rectangle::_update_vertices()
{
    _vertices[0].x = _center.x - _width/2.0f;
    _vertices[0].y = _center.y - _height/2.0f;

    _vertices[1].x = _center.x - _width/2.0f;
    _vertices[1].y = _center.y + _height/2.0f;

    _vertices[2].x = _center.x + _width/2.0f;
    _vertices[2].y = _center.y + _height/2.0f;

    _vertices[3].x = _center.x + _width/2.0f;
    _vertices[3].y = _center.y - _height/2.0f;
}

Rectangle::Rectangle(const float x1, const float y1, const float x2, const float y2)
{
    _center.x = (x1 + x2) / 2.0f;
    _center.y = (y1 + y2) / 2.0f;
    _width    = x2 - x1;
    _height   = y2 - y1;

    _update_vertices();
}

Rectangle::Rectangle( const vec2_t& center, const float width, const float height): _center(center), _width(width), _height(height)
{
    _update_vertices();
}

void Rectangle::SetCenter(const vec2_t &center)
{
    _center = center;
    _update_vertices();
}

void Rectangle::SetWidth(const float width)
{
    _width = width;
    _update_vertices();
}

void Rectangle::SetHeight(const float height)
{
    _height = height;
    _update_vertices();
}

void Rectangle::Draw(GLenum mode)
{
    glVertexPointer( 2, GL_FLOAT, 0, _vertices);
    glDrawArrays( mode, 0, 4);
}


TextLabel::TextLabel(const vec2_t &center, const float width, const float height):
    Rectangle(center,width,height)
{
    glGenTextures(1, &_texture);
}

TextLabel::TextLabel(const float x1, const float y1, const float x2, const float y2):
    Rectangle(x1,y1,x2,y2)
{
    glGenTextures(1,&_texture);
}

void TextLabel::Draw(GLenum mode)
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glColor4f(.5f,1.0f,1.0f,1.0f);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexCoordPointer(2, GL_FLOAT, 0, _texcoords);

    Rectangle::Draw( GL_TRIANGLE_FAN );

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    //cout << gluErrorString(glGetError()) << endl;

}

void TextLabel::SetText(const string &text)
{
    stringstream s;
    s << "label:" << text;
    _text = text;
    TextRenderer::I().Text2Texture(_texture, s.str());
}

const vec2_t TextLabel::_texcoords[4] = { {0,1},{0,0},{1,0},{1,1} };
