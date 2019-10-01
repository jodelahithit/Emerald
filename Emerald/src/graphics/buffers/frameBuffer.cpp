#include "stdafx.h"

FrameBuffer::FrameBuffer(String name, FBOScale scale, Color& clearColor) : m_name(name), m_width(0), m_height(0), m_color(clearColor) {
	m_scale = scale;
	m_realWidth = GetApplication()->GetWidth();
	m_realHeight = GetApplication()->GetHeight();
	m_width = FBOScaleToFloat(m_scale) * m_realWidth;
	m_height = FBOScaleToFloat(m_scale) * m_realHeight;

	GL(glGenFramebuffers(1, &m_fbo));
	GL(glGenRenderbuffers(1, &m_dbo));

	GL(glBindRenderbuffer(GL_RENDERBUFFER, m_dbo));
	GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height));

	GL(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
	GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_dbo));
	if (CheckStatus()) LOG("[~cBuffers~x] Created ~1%s~x framebuffer", m_name.c_str());
}

void FrameBuffer::Resize(uint width, uint height) {
	m_realWidth = width;
	m_realHeight = height;
	m_width = FBOScaleToFloat(m_scale) * m_realWidth;
	m_height = FBOScaleToFloat(m_scale) * m_realHeight;
	GL(glBindRenderbuffer(GL_RENDERBUFFER, m_dbo));
	GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height));
	GL(glBindRenderbuffer(GL_RENDERBUFFER, 0));

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
	GL(glDeleteTextures(1, &m_dbo));
}

bool FrameBuffer::CheckStatus() {
	GL(GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("[~cBuffers~x] ~rFramebuffer %s failed: %s", m_name.c_str(), GLUtils::GetFBOStatus(status));
		return false;
	} else return true;
}

AssetRef<Texture> FrameBuffer::AddColorBuffer(const String& name, const TextureParameters& params) {
	Texture* texture = NEW(Texture(m_width, m_height, params));
	m_textures.push_back(texture);
	m_textureNames.push_back(name);

	Bind();

	GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_colorAttachments, GL_TEXTURE_2D, texture->GetHandle(), 0));

	m_colorAttachments++;
	GL(glDrawBuffers(m_colorAttachments, drawBuffers));

	GL(GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (status != GL_FRAMEBUFFER_COMPLETE) LOG_ERROR("[~cBuffers~x] ~radding attachment to %s failed: %s", m_name.c_str(), GLUtils::GetFBOStatus(status));
	Unbind();

	return texture;
}

void FrameBufferManager::OnImGUI() {
	if (ImGui::BeginTabItem("Framebuffers")) {
		ImGui::Columns(3, NULL, false);
		vector<String> m_fboTextureNames;
		int index = 0;
		for (FrameBuffer* fbo : m_frameBuffers) {
			ImGui::Separator();
			ImGui::Text("%s", fbo->GetName().c_str());
			//if (ImGui::ColorPicker3("Color", (float*)&fbo->m_color)) {
			//	fbo->SetClearColor(fbo->m_color);
			//}
			m_fboTextureNames = fbo->GetTextureNames();

			int selected = (int)fbo->GetScale();
			static String_t scales[] = { "Full", "Half", "Quarter" };
			if (ImGui::Combo(Format_t("Scale##%d", index++), &selected, scales, 3)) fbo->SetScale(FBOScale(selected));
			int i = 0;
			for (AssetRef<Texture>& tex : fbo->GetTextures()) {
				ImGui::NextColumn();
				if (ImGui::GetColumnIndex() == 0) ImGui::NextColumn();
				if (ImGui::ImageButton(tex->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) m_selectedTexture = tex;
				ImGui::Tooltip(fbo->GetName().c_str());
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
	GL(glViewport(0, 0, GetApplication()->GetWidth(), GetApplication()->GetHeight()));
}