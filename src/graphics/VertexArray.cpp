//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2011 Ulrich von Zadow
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Current versions can be found at www.libavg.de
//

#include "VertexArray.h"

#include "GLContext.h"
#include "SubVertexArray.h"

#include "../base/Exception.h"
#include "../base/WideLine.h"
#include "../base/ObjectCounter.h"

#include <iostream>
#include <stddef.h>
#include <string.h>

using namespace std;
using namespace boost;

namespace avg {

VertexArray::VertexArray(int reserveVerts, int reserveIndexes)
    : VertexData(reserveVerts, reserveIndexes)
{
    if (reserveVerts != MIN_VERTEXES || reserveIndexes != MIN_INDEXES) {
        glproc::GenBuffers(1, &m_GLVertexBufferID);
        glproc::GenBuffers(1, &m_GLIndexBufferID);
    } else {
        GLContext* pContext = GLContext::getCurrent();
        m_GLVertexBufferID = pContext->getVertexBufferCache().getBuffer();
        m_GLIndexBufferID = pContext->getIndexBufferCache().getBuffer();
    }
}

VertexArray::~VertexArray()
{
    GLContext* pContext = GLContext::getCurrent();
    if (pContext) {
        if (getReserveVerts() == MIN_VERTEXES) {
            pContext->getVertexBufferCache().returnBuffer(m_GLVertexBufferID);
        } else {
            glproc::DeleteBuffers(1, &m_GLVertexBufferID);
        }
        if (getReserveIndexes() == MIN_INDEXES) {
            pContext->getIndexBufferCache().returnBuffer(m_GLIndexBufferID);
        } else {
            glproc::DeleteBuffers(1, &m_GLIndexBufferID);
        }
    }
}

void VertexArray::update()
{
    if (hasDataChanged()) {
        glproc::BindBuffer(GL_ARRAY_BUFFER, m_GLVertexBufferID);
        glproc::BufferData(GL_ARRAY_BUFFER, getReserveVerts()*sizeof(T2V3C4Vertex), 0, 
                GL_DYNAMIC_DRAW);
        void * pBuffer = glproc::MapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(pBuffer, getVertexPointer(), getNumVerts()*sizeof(T2V3C4Vertex));
        glproc::UnmapBuffer(GL_ARRAY_BUFFER);
        
        glproc::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_GLIndexBufferID);
        glproc::BufferData(GL_ELEMENT_ARRAY_BUFFER, 
            getReserveIndexes()*sizeof(unsigned int), 0, GL_DYNAMIC_DRAW);
        pBuffer = glproc::MapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(pBuffer, getIndexPointer(), getNumIndexes()*sizeof(unsigned int));
        glproc::UnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        
        GLContext::getCurrent()->checkError("VertexArray::update");
    }
    resetDataChanged();
}

void VertexArray::activate()
{
    glproc::BindBuffer(GL_ARRAY_BUFFER, m_GLVertexBufferID);
    glproc::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_GLIndexBufferID);
    glTexCoordPointer(2, GL_FLOAT, sizeof(T2V3C4Vertex), 0);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(T2V3C4Vertex), 
            (void *)(offsetof(T2V3C4Vertex, m_Color)));
    glVertexPointer(3, GL_FLOAT, sizeof(T2V3C4Vertex),
            (void *)(offsetof(T2V3C4Vertex, m_Pos)));
    GLContext::getCurrent()->checkError("VertexArray::draw:1");
}

void VertexArray::draw()
{
    update();
    glproc::BindBuffer(GL_ARRAY_BUFFER, m_GLVertexBufferID);
    glTexCoordPointer(2, GL_FLOAT, sizeof(T2V3C4Vertex), 0);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(T2V3C4Vertex), 
            (void *)(offsetof(T2V3C4Vertex, m_Color)));
    glVertexPointer(3, GL_FLOAT, sizeof(T2V3C4Vertex),
            (void *)(offsetof(T2V3C4Vertex, m_Pos)));
    GLContext::getCurrent()->checkError("VertexArray::draw:1");

    glproc::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_GLIndexBufferID);
    // TODO: glDrawRangeElements is allegedly faster.
    glDrawElements(GL_TRIANGLES, getNumIndexes(), GL_UNSIGNED_INT, 0);
    GLContext::getCurrent()->checkError( "VertexArray::draw():2");
}

void VertexArray::draw(unsigned startIndex, unsigned numIndexes)
{
    glDrawElements(GL_TRIANGLES, numIndexes, GL_UNSIGNED_INT, 
            (void *)(startIndex*sizeof(unsigned)));
    GLContext::getCurrent()->checkError( "VertexArray::draw()");
}

SubVertexArrayPtr VertexArray::startSubVA()
{
    return SubVertexArrayPtr(new SubVertexArray(this, getNumVerts(), getNumIndexes()));
}

}

