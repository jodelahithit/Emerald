#include "stdafx.h"

FrameBuffer::FrameBuffer(String name, FBOScale scale, Color& clearColor) : m_name(name), m_scale(scale), m_width(0), m_height(0), m_color(clearColor), m_hasDepth(false), m_hasStencil(false) {
	m_realWidth = GetApp()->GetWidth();
	m_realHeight = GetApp()->GetHeight();
	m_width = (uint)(FBOScaleToFloat(m_scale) * m_realWidth);
	m_height = (uint)(FBOScaleToFloat(m_scale) * m_realHeight);

	GL(glGenFramebuffers(1, &m_fbo));
	GL(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
	GL(glClearColor(m_color.R, m_color.G, m_color.B, m_color.A));
}

FrameBuffer::FrameBuffer(String name, uint width, uint height, Color& clearColor) : m_name(name), m_scale(FBOScale::STATIC), m_width(0), m_height(0), m_color(clearColor), m_hasDepth(false), m_hasStencil(false) {
	m_realWidth = width;
	m_realHeight = height;
	m_width = (uint)(FBOScaleToFloat(m_scale) * m_realWidth);
	m_height = (uint)(FBOScaleToFloat(m_scale) * m_realHeight);

	GL(glGenFramebuffers(1, &m_fbo));
	GL(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
	GL(glClearColor(m_color.R, m_color.G, m_color.B, m_color.A));
}

void FrameBuffer::Resize(uint width, uint height) {
	if (m_scale == FBOScale::STATIC) return;
	m_realWidth = width;
	m_realHeight = height;
	m_width = (uint)(FBOScaleToFloat(m_scale) * m_realWidth);
	m_height = (uint)(FBOScaleToFloat(m_scale) * m_realHeight);

	for (Texture* texture : m_textures) {
		texture->Resize(m_width, m_height);
	}

	CheckStatus();
}

FrameBuffer::~FrameBuffer() {
	GL(glDeleteFramebuffers(1, &m_fbo));
	for (AssetRef<Texture> texture : m_textures) {
		DELETE(texture.Get());
	}
}

bool FrameBuffer::CheckStatus() {
	GL(GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("[~cBuffers~x] ~rFramebuffer %s failed: %s", m_name.c_str(), GLUtils::GetFBOStatus(status));
		return false;
	} else return true;
}

AssetRef<Texture> FrameBuffer::AddBuffer(const String& name, const TextureParameters& params, FBOAttachment type) {
	Texture* texture = NEW(Texture(m_width, m_height, false, params));
	m_textures.push_back(texture);
	m_textureNames.push_back(name);

	Bind();

	if (type == FBOAttachment::COLOR) {
		GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_colorAttachments, GL_TEXTURE_2D, texture->GetHandle(), 0));
		GL(glDrawBuffers(++m_colorAttachments, drawBuffers));
	} else if (type == FBOAttachment::DEPTH) {
		GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, texture->GetHandle(), 0));
		m_hasDepth = true;
	} else if (type == FBOAttachment::STENCIL) {
		GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->GetHandle(), 0));
		m_hasStencil = true;
	} else if (type == FBOAttachment::DEPTHSTENCIL) {
		GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->GetHandle(), 0));
		m_hasDepth = true;
		m_hasStencil = true;
	}

	GL(GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (status != GL_FRAMEBUFFER_COMPLETE) LOG_ERROR("[~cBuffers~x] ~radding attachment to %s failed: %s", m_name.c_str(), GLUtils::GetFBOStatus(status));
	Unbind();

	return texture;
}

void FrameBuffer::Blit(FrameBuffer* targetFBO) {
	GL(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo));
	GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFBO ? targetFBO->GetHandle() : 0));
	GL(glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, targetFBO ? targetFBO->GetWidth() : GetApp()->GetWidth(), targetFBO ? targetFBO->GetHeight() : GetApp()->GetHeight(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST));
}

void FrameBuffer::BlitDepthOnly(FrameBuffer* targetFBO) {
	GL(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo));
	GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFBO ? targetFBO->GetHandle() : 0));
	GL(glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, targetFBO ? targetFBO->GetWidth() : GetApp()->GetWidth(), targetFBO ? targetFBO->GetHeight() : GetApp()->GetHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST));
}


void FrameBufferManager::OnImGUI() {
	if (ImGui::BeginTabItem("Framebuffers")) {
		ImGui::Columns(3, NULL, false);
		vector<String> m_fboTextureNames;
		int index = 0;
		for (FrameBuffer* fbo : m_frameBuffers) {
			ImGui::Separator();
			ImGui::Text("%s", fbo->GetName().c_str());
			m_fboTextureNames = fbo->GetTextureNames();

			int selected = (int)fbo->GetScale();
			static String_t scales[] = { "Static", "Full", "Half", "Quarter", "One Fifth" };
			if (ImGui::Combo(Format_t("Scale##%d", index++), &selected, scales, NUMOF(scales))) fbo->SetScale(FBOScale(selected));
			int i = 0;
			for (AssetRef<Texture>& tex : fbo->GetTextures()) {
				ImGui::NextColumn();
				if (ImGui::GetColumnIndex() == 0) ImGui::NextColumn();
				if (ImGui::ImageButton(tex->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) m_selectedTexture = tex;
				ImGui::Tooltip(m_fboTextureNames[i].c_str());
				ImGui::Text("%s", m_fboTextureNames[i++].c_str());
				ImGui::Text("%d x %d", tex->GetWidth(), tex->GetHeight());
				ImGui::Text(tex->GetTextureParams().GetAsString().c_str());
			}
			while (ImGui::GetColumnIndex() > 0) ImGui::NextColumn();
		}
		ImGui::Columns(1);
		ImGui::Separator();

		ImGui::EndTabItem();
	}
}

void FrameBufferManager::BindDefaultFBO() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GL(glViewport(0, 0, GetApp()->GetWidth(), GetApp()->GetHeight()));
}
