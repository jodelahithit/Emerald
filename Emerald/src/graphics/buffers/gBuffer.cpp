#include "stdafx.h"

void GBuffer::Initialize() {
    m_fbo = new FrameBuffer(m_width, m_height);
    m_fbo->Bind();

    m_colorTexture = new Texture(m_width, m_height, TextureParameters(RGBA, LINEAR, REPEAT, T_UNSIGNED_BYTE));
    m_normalTexture = new Texture(m_width, m_height, TextureParameters(RGBA16, LINEAR, REPEAT, T_FLOAT));
    m_positionTexture = new Texture(m_width, m_height, TextureParameters(RGBA32, LINEAR, REPEAT, T_FLOAT));

    GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture->GetHandle(), 0));
    GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normalTexture->GetHandle(), 0));
    GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_positionTexture->GetHandle(), 0));

    GLenum drawBuffers[3] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2
    };

    GL(glDrawBuffers(3, drawBuffers));

    GL(GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
    if (status != GL_FRAMEBUFFER_COMPLETE) LOG_ERROR("[Buffers] initializing GBuffer failed %d", status);

    m_fbo->Unbind();
}

void GBuffer::BindTextures() {
    m_colorTexture->Bind(0);
    m_normalTexture->Bind(1);
    m_positionTexture->Bind(2);
}