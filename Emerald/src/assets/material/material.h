#pragma once

//class Material {
//protected:
//	friend class MaterialManager;
//
//public:
//	virtual void Bind(AssetRef<Shader> shader) = 0;
//	//virtual void Unbind() = 0;
//};

class MaterialMember {
public:
	String m_name;
	String m_uniform;
	ShaderPropertyType m_type;
	uint m_location;
	virtual void Set(Shader* shader) = 0;
	virtual void OnImGui() = 0;
	void GetUniformLocation(Shader* shader) {
		m_location = shader->GetUniformLocation(m_uniform.c_str());
	}
};

class MaterialMemberRGB : public MaterialMember {
public:
	Color m_value;
	void Set(Shader* shader) override {
		shader->SetVec3(m_location, glm::vec3(m_value.R, m_value.G, m_value.B));
	}

	void OnImGui() override {
		UI::Color3(m_name.c_str(), &m_value);
	}
};

class MaterialMemberRGBA : public MaterialMember {
public:
	Color m_value;
	void Set(Shader* shader) override {
		shader->SetColor(m_location, m_value);
	}

	void OnImGui() override {
		UI::Color4(m_name.c_str(), &m_value);
	}
};

class MaterialMemberFloat : public MaterialMember {
public:
	float m_value;
	void Set(Shader* shader) override {
		shader->SetFloat(m_location, m_value);
	}

	void OnImGui() override {
		UI::Float(m_name.c_str(), &m_value);
	}
};

class MaterialMemberRange : public MaterialMember {
public:
	float m_value;
	float m_min;
	float m_max;
	void Set(Shader* shader) override {
		shader->SetFloat(m_location, m_value);
	}

	void OnImGui() override {
		UI::Float(m_uniform.c_str(), &m_value, m_min, m_max);
	}
};

class MaterialMemberInt : public MaterialMember {
public:
	int m_value;
	void Set(Shader* shader) override {
		shader->SetInt(m_location, m_value);
	}

	void OnImGui() override {
		UI::Int(m_name.c_str(), &m_value);
	}
};

class MaterialMemberBool : public MaterialMember {
public:
	bool m_value;
	void Set(Shader* shader) override {
		shader->SetInt(m_location, m_value);
	}

	void OnImGui() override {
		UI::Bool(m_name.c_str(), &m_value);
	}
};

class MaterialMemberVec2 : public MaterialMember {
public:
	glm::vec2 m_value;
	void Set(Shader* shader) override {
		shader->SetVec2(m_location, m_value);
	}

	void OnImGui() override {
		UI::Vec2(m_name.c_str(), &m_value);
	}
};

class MaterialMemberVec3 : public MaterialMember {
public:
	glm::vec3 m_value;
	void Set(Shader* shader) override {
		shader->SetVec3(m_location, m_value);
	}

	void OnImGui() override {
		UI::Vec3(m_name.c_str(), &m_value);
	}
};

class MaterialMemberVec4 : public MaterialMember {
public:
	glm::vec4 m_value;
	void Set(Shader* shader) override {
		shader->SetVec4(m_location, m_value);
	}

	void OnImGui() override {
		UI::Vec4(m_name.c_str(), &m_value);
	}
};

class MaterialMemberTexture : public MaterialMember {
public:
	ShaderPropertyTextureType m_textureType;
	Texture* m_texture;
	int m_slot;
	void Set(Shader* shader) override {
		shader->SetInt(m_location, m_slot);
		m_texture->Bind(m_slot);
	}

	void OnImGui() override {
		ImGui::AlignTextToFramePadding();
		ImGui::Text(m_name.c_str());
		float width = ImGui::GetContentRegionAvail().x;
		ImGui::SameLine(width / 2);
		ImGui::PushItemWidth(width / 2);
		ImGui::Image(m_texture->GetImGuiHandle(), ImVec2(120, 120), ImVec2(0, 1), ImVec2(1, 0));
	}
};
class Material {
private:
	static uint s_boundMaterial;
	String m_name;
	Shader* m_shader;
	uint m_ID;
	vector<MaterialMember*> m_members;
	vector<MaterialCallback*> m_callbacksOnBind;
	vector<MaterialCallback*> m_callbacksOnInstance;

	Material(const String& name, Shader* shader, uint ID);

	friend class MaterialManager;
public:

	~Material() {
		for (auto& member : m_members) DELETE(member);
		for (auto& callback : m_callbacksOnInstance) DELETE(callback);
		for (auto& callback : m_callbacksOnBind) DELETE(callback);

		m_callbacksOnInstance.clear();
		m_callbacksOnBind.clear();
		m_members.clear();
	}

	const String& GetName() { return m_name; }

	Shader* GetShader() { return m_shader; }

	void AddCallback(MaterialCallbackType type, MaterialCallback* callback) {
		callback->SetUniformLocation(m_shader);
		if (type == MaterialCallbackType::ONINSTANCE) {
			m_callbacksOnInstance.push_back(callback);
		} else m_callbacksOnBind.push_back(callback);
	}

	void Bind();
	void BindInstance();

	void OnImGui() {
		for (auto& member : m_members) {
			member->OnImGui();
		}
	}

	uint GetID() const { return m_ID; }

	//#Dirty!
	void SetRoughnessIfAvailable(Texture* tex) {
		if (!tex)return;
		for (auto& member : m_members) {
			if (member->m_type == ShaderPropertyType::TEXTURE) {
				if (member->m_uniform.compare("_Roughness") == 0) {
					((MaterialMemberTexture*)member)->m_texture = tex;
				}
			}
		}
	}
	void SetAlbedoIfAvailable(Texture* tex) {
		if (!tex)return;
		for (auto& member : m_members) {
			if (member->m_type == ShaderPropertyType::TEXTURE) {
				if (member->m_uniform.compare("_Albedo") == 0) {
					((MaterialMemberTexture*)member)->m_texture = tex;
				}
			}
		}
	}
	void SetNormalIfAvailable(Texture* tex) {
		if (!tex)return;
		for (auto& member : m_members) {
			if (member->m_type == ShaderPropertyType::TEXTURE) {
				if (member->m_uniform.compare("_Normal") == 0) {
					((MaterialMemberTexture*)member)->m_texture = tex;
				}
			}
		}
	}
	void SetMetallicIfAvailable(Texture* tex) {
		if (!tex)return;
		for (auto& member : m_members) {
			if (member->m_type == ShaderPropertyType::TEXTURE) {
				if (member->m_uniform.compare("_Metallic") == 0) {
					((MaterialMemberTexture*)member)->m_texture = tex;
				}
			}
		}
	}

	void SetPBR(const String& name);
};