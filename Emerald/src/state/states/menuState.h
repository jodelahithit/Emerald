#pragma once

struct DrawableLine {
	Vector3 x1 = Vector3(0);
	Vector3 x2 = Vector3(0);
	float amount = 0;
	float duration = 0;
	bool enabled = false;
};

static float eta = 0.0001f;
static float alpha = 0.0f;

static float halfGridSize = 14;
static int gridSize = 28;
static int totalGridSize = 28 * 28;
static DrawableLine** drawableLines;
static int drawableCount = 4096;
static int lineRendererIndex = 0;
static void DrawLine(Vector3 x1, Vector3 x2, float amount) {
	DrawableLine* linea;
	for (int i = 0; i < drawableCount; i++) {
		DrawableLine* line = drawableLines[i];
		if (!line->enabled) {
			line->enabled = true;
			line->x1 = x1;
			linea = line;
			line->x2 = x2;
			line->amount = amount;
			line->duration = 1000;
			break;
		}
	}
}

struct Topology {
	int count;
	int layers;
};

class Neuron;

struct Connection {
	double weight;
	double deltaWeight = 0;
	Neuron* neuron;
	double passingValue = 1;
};

class Neuron {
public:
	static double randomWeight() { return rand() / double(RAND_MAX); }

	Neuron(uint numOutputs, uint myIndex, float x, float y, float z) : m_myIndex(myIndex) {
		Model* model = NEW(Model(GetAssetManager()->Get<Model>("Sphere")->GetMeshes()[0]->Copy()));
		m_material = GetMaterialManager()->CreateUnsafe<BasicMaterial>();
		m_material->SetAlbedo(GetAssetManager()->Get<Texture>("White"));
		m_material->SetSpecular(GetAssetManager()->Get<Texture>("Spec"));
		model->SetMaterial(m_material);
		m_entity = NEW(Entity(model));
		m_entity->m_position = Vector3(x, y, z);
		for (uint c = 0; c < numOutputs; ++c) {
			m_outputWeights.push_back(Connection());
			m_outputWeights.back().weight = randomWeight();
		}
	}

	void setOutputValue(double val) { m_outputValue = val; }
	double getOutputValue() const { return m_outputValue; }
	void feedForward(vector<Neuron>& previousLayer) {
		double sum = 0.0;

		for (uint n = 0; n < previousLayer.size(); ++n) {
			Neuron& neuron = previousLayer[n];
			double passingValue = previousLayer[n].m_outputValue * previousLayer[n].m_outputWeights[m_myIndex].weight;
			sum += passingValue;
			neuron.m_outputWeights[m_myIndex].passingValue = passingValue;
			//if (passingValue > 0) {
			//	if (lineRendererIndex++ % 111 == 0) {
			//		DrawLine(m_entity->m_position, neuron.m_entity->m_position, passingValue);
			//	}
			//}
		}

		m_outputValue = transferFunction(sum);
	}

	double sumDOW(const vector<Neuron>& nextLayer) const {
		double sum = 0.0;
		for (uint n = 0; n < nextLayer.size() - 1; ++n) {
			sum += m_outputWeights[n].weight * nextLayer[n].m_gradient;
		}
		return sum;
	}
	void calcOutputGradients(double targetVal) {
		double delta = targetVal - m_outputValue;
		m_gradient = delta * Neuron::transferFunctionDerivative(m_outputValue);
	}

	void calcHiddenGradients(const vector<Neuron>& nextLayer) {
		double dow = sumDOW(nextLayer);
		m_gradient = dow * Neuron::transferFunctionDerivative(m_outputValue);
	}

	void updateInputWeights(vector<Neuron>& prevLayer) {
		for (uint n = 0; n < prevLayer.size(); ++n) {
			Neuron& neuron = prevLayer[n];
			double oldDeltaWeight = neuron.m_outputWeights[m_myIndex].deltaWeight;

			double newDeltaWeight = eta * neuron.getOutputValue() * m_gradient + alpha * oldDeltaWeight;
			neuron.m_outputWeights[m_myIndex].deltaWeight = newDeltaWeight;
			neuron.m_outputWeights[m_myIndex].weight += newDeltaWeight;
		}
	}

	void setConnections(vector<Neuron>& prevLayer) {
		for (uint n = 0; n < prevLayer.size(); ++n) {
			Neuron& neuron = prevLayer[n];
			neuron.m_outputWeights[m_myIndex].neuron = this;
		}
	}

	void Draw(AssetRef<Shader>& shader, bool renderType) {
		float value = renderType ? m_gradient : m_outputValue;

		Color color(0, 0, 0, 1);
		if (m_outputValue < 0) color.B = -value;
		else color.R = value;

		m_material->m_color = color;
		m_entity->Draw(shader, GL_PATCHES);

		for (auto& c : m_outputWeights) {
			float value = renderType ? c.weight : c.passingValue;
			Color color(0, 0, 0, 1);
			if (c.weight < 0) color.B = -value;
			else color.R = value;
			if (lineRendererIndex++ % 111 == 0) {
				GetLineRenderer()->Submit(m_entity->m_position, c.neuron->m_entity->m_position, color);
			}
		}
	}

	void Export(nlohmann::json& jsonOb) {
		for (int i = 0; i < m_outputWeights.size(); i++) {
			jsonOb[to_string(i)]["w"] = m_outputWeights[i].weight;
			jsonOb[to_string(i)]["d"] = m_outputWeights[i].deltaWeight;
			jsonOb[to_string(i)]["v"] = m_outputValue;
			jsonOb[to_string(i)]["g"] = m_gradient;
		}
	}

	void Import(nlohmann::json& jsonOb) {
		for (int i = 0; i < m_outputWeights.size(); i++) {
			m_outputWeights[i].weight = jsonOb[to_string(i)]["w"];
			m_outputWeights[i].deltaWeight = jsonOb[to_string(i)]["d"];
			m_outputValue = jsonOb[to_string(i)]["v"];
			m_gradient = jsonOb[to_string(i)]["g"];
		}
	}

private:
	double m_outputValue;
	double m_gradient;

	uint m_myIndex;
	vector<Connection> m_outputWeights;
	AssetRef<Entity> m_entity;
	AssetRef<BasicMaterial> m_material;
	static double transferFunction(double x) {
		return tanh(x);
	}

	static double transferFunctionDerivative(double x) {
		return 1.0 - tanh(x) * tanh(x);
	}
};

class NeuralNetwork {
public:
	double m_recentAverageError;

	NeuralNetwork(const vector<Topology>& topology) {
		uint numLayers = topology.size();
		for (int layerNum = 0; layerNum < numLayers; layerNum++) {
			m_layers.push_back(vector<Neuron>());
			uint numOutputs = layerNum == topology.size() - 1 ? 0 : topology[layerNum + 1].count;
			for (int neuronNum = 0; neuronNum <= topology[layerNum].count; neuronNum++) {
				Topology topo = topology[layerNum];
				float xPos = ((int)((float)neuronNum / topo.layers)) - (((float)topo.count / topo.layers) / 2);
				int layerSizeX = topo.layers;
				float yPos = (neuronNum % topo.layers) - ((float)layerSizeX / 2);

				//float randomZ = Math::RandomFloat(10, (numLayers - 2) * 50);
				//float randomY = Math::RandomFloat(-gridSize * 2, gridSize * 2);
				//float randomX = Math::RandomFloat(-gridSize * 2, gridSize * 2);
				//
				//if (layerNum == 0 || layerNum == numLayers - 1)
				m_layers[layerNum].push_back(Neuron(numOutputs, neuronNum, yPos, xPos, layerNum * 10 + 0.75f));
				//else
				//	m_layers[layerNum].push_back(Neuron(numOutputs, neuronNum, randomX, randomY, randomZ));

			}
			m_layers.back().back().setOutputValue(1.0);
		}

		for (uint layerNum = m_layers.size() - 1; layerNum > 0; --layerNum) {
			vector<Neuron>& layer = m_layers[layerNum];
			vector<Neuron>& previousLayer = m_layers[layerNum + -1];

			for (uint n = 0; n < layer.size() - 1; ++n) {
				layer[n].setConnections(previousLayer);
			}
		}
	}

	void Export() {
		nlohmann::json jsonOb;
		for (int i = 0; i < m_layers.size() - 1; i++) {
			for (int j = 0; j < m_layers[i].size(); j++) {
				m_layers[i][j].Export(jsonOb[to_string(i)][to_string(j)]);
			}
		}
		FileSystem::SaveJsonToFile(jsonOb, "neuralNetwork");
	}

	void Import() {
		try {
			nlohmann::json jsonOb = FileSystem::LoadJsonFromFile("neuralNetwork");
			for (int i = 0; i < m_layers.size() - 1; i++) {
				for (int j = 0; j < m_layers[i].size(); j++) {
					m_layers[i][j].Import(jsonOb[to_string(i)][to_string(j)]);
				}
			}
		} catch (...) {}
	}
	void feedForward(const vector<double>& inputValues) {
		if (inputValues.size() != m_layers[0].size() - 1) LOG_ERROR("Inconsistent input values!");
		for (uint i = 0; i < inputValues.size(); i++) {
			m_layers[0][i].setOutputValue(inputValues[i]);
		}

		for (uint layerNum = 1; layerNum < m_layers.size(); layerNum++) {
			vector<Neuron>& previousLayer = m_layers[layerNum - 1];
			for (uint n = 0; n < m_layers[layerNum].size() - 1; ++n) {
				m_layers[layerNum][n].feedForward(previousLayer);
			}
		}
	}
	void backProp(const vector<double>& targetValues) {
		if (targetValues.size() != m_layers.back().size() - 1) LOG_ERROR("Inconsistent target values!");

		vector<Neuron>& outputLayer = m_layers.back();
		m_error = 0.0;

		for (uint n = 0; n < outputLayer.size() - 1; ++n) {
			double delta = targetValues[n] - outputLayer[n].getOutputValue();
			m_error += delta * delta;
		}

		m_error /= outputLayer.size() - 1;
		m_error = sqrt(m_error);

		m_recentAverageError = (m_recentAverageError * m_recentAverageSmoothingFactor + m_error) / (m_recentAverageSmoothingFactor + 1.0);

		for (uint n = 0; n < outputLayer.size() - 1; ++n) {
			outputLayer[n].calcOutputGradients(targetValues[n]);
		}

		for (uint layerNum = m_layers.size() - 2; layerNum > 0; --layerNum) {
			vector<Neuron>& hiddenLayer = m_layers[layerNum];
			vector<Neuron>& nextLayer = m_layers[layerNum + 1];
			for (uint n = 0; n < hiddenLayer.size(); ++n) {
				hiddenLayer[n].calcHiddenGradients(nextLayer);
			}
		}

		for (uint layerNum = m_layers.size() - 1; layerNum > 0; --layerNum) {
			vector<Neuron>& layer = m_layers[layerNum];
			vector<Neuron>& previousLayer = m_layers[layerNum - 1];

			for (uint n = 0; n < layer.size() - 1; ++n) {
				layer[n].updateInputWeights(previousLayer);
			}
		}
	}
	void getResults(vector<double>& resultValues) const {
		resultValues.clear();
		for (uint n = 0; n < m_layers.back().size() - 1; ++n) {
			resultValues.push_back(m_layers.back()[n].getOutputValue());
		}
	}

	void Draw(AssetRef<Shader>& shader, bool renderType) {

		for (auto& layer : m_layers) {
			for (int i = 0; i < layer.size() - 1; i++) {
				layer[i].Draw(shader, renderType);
			}
		}
	}

private:
	vector<vector<Neuron>> m_layers;
	double m_error;
	double m_recentAverageSmoothingFactor = 30;
};

static vector<vector<double>> outputValues;

struct Number {
	int number;
	vector<double> data;
};

struct Pixel {
	AssetRef<Entity> m_entity;
	bool m_enabled;
};

class MenuState : public State {
private:

	String m_name = "Menu";
	AssetRef<Shader> m_shader;
	vector<Topology> topology = { {totalGridSize, gridSize}, {529, 23}, {256, 16}, {121, 11}, {10, 1} };
	NeuralNetwork* m_network;
	bool renderType = false;
	vector<Pointlight> m_pointLights;
	vector<Pixel> m_cubes;
	GroundRaycast* cast;
	Vector3 m_rayCastPos;
	bool down = false;
	bool downState;
	vector<double> m_inputValues;
	void LoadNumbers();
	vector<Number> m_numbers;
public:
	const String& GetName() override { return m_name; }

	void Initialize() override {
		LoadNumbers();
		for (int i = 0; i < totalGridSize; i++) m_inputValues.push_back(1.0);
		drawableLines = new DrawableLine*[drawableCount];
		for (int i = 0; i < drawableCount; i++) {
			drawableLines[i] = new DrawableLine();
		}
		cast = new GroundRaycast();
		loop(y, gridSize) {
			loop(x, gridSize) {
				Entity* e = NEW(Entity(NEW(Model(GetAssetManager()->Get<Model>("Box")->GetMeshes()[0]->Copy()))));
				BasicMaterial* mat = GetMaterialManager()->CreateUnsafe<BasicMaterial>();
				mat->SetAlbedo(GetTextureManager()->GetWhiteTexture());
				mat->SetEmission(GetTextureManager()->GetWhiteTexture());
				mat->m_emissionStrength = 1;
				e->m_position = Vector3(x - halfGridSize, y - halfGridSize, 0);
				e->m_scale = Vector3(0.5f);
				e->m_model->SetMaterial(mat);
				m_cubes.push_back({ e, true });
			}
		}
		m_pointLights.push_back(Pointlight(Vector3(-10, 5, -5), 100, Color::White() * 10));
		m_pointLights.push_back(Pointlight(Vector3(10, 5, -5), 100, Color::White() * 10));
		m_pointLights.push_back(Pointlight(Vector3(0, 0, -10), 100, Color::White() * 10));
		m_shader = GetShaderManager()->Get("Geometry");

		m_network = NEW(NeuralNetwork(topology));

		GetCamera()->m_position = Vector3(0.0f, 0.0f, -30.0f);
		GetCamera()->m_rotation = Vector3(0, Math::PI, 0.0f);
		GetPipeline()->m_directionalLight.m_direction = Vector3(Math::HALF_PI, 0.0f, 0.0f);
	}
	void Update(const TimeStep& time) override {

		//if (KeyJustDown(VK_0)) BackPropNumber(m_network, 0);
		//if (KeyJustDown(VK_1)) BackPropNumber(m_network, 1);
		//if (KeyJustDown(VK_2)) BackPropNumber(m_network, 2);
		//if (KeyJustDown(VK_3)) BackPropNumber(m_network, 3);
		//if (KeyJustDown(VK_4)) BackPropNumber(m_network, 4);
		//if (KeyJustDown(VK_5)) BackPropNumber(m_network, 5);
		//if (KeyJustDown(VK_6)) BackPropNumber(m_network, 6);
		//if (KeyJustDown(VK_7)) BackPropNumber(m_network, 7);
		//if (KeyJustDown(VK_8)) BackPropNumber(m_network, 8);
		//if (KeyJustDown(VK_9)) BackPropNumber(m_network, 9);

		//if (KeyJustDown(VK_1)) BackPropTriangle(m_network);
		//if (KeyJustDown(VK_2)) BackPropCross(m_network);
		//if (KeyJustDown(VK_3)) BackPropCircle(m_network);

		GetCamera()->Update(time);
		cast->Get(GetCamera());
		m_rayCastPos = cast->GetZ(-0.75f);

		if (ButtonDown(VK_MOUSE_LEFT)) {
			if (Math::Within(m_rayCastPos.x, -halfGridSize - 0.5f, halfGridSize - 0.5f)) {
				if (Math::Within(m_rayCastPos.y, -halfGridSize - 0.5f, halfGridSize - 0.5f)) {
					int posX = (m_rayCastPos.x + 0.5f - (m_rayCastPos.x < 0 ? 1 : 0));
					int posY = (m_rayCastPos.y + 0.5f - (m_rayCastPos.y < 0 ? 1 : 0));
					int index = ((posY + halfGridSize) * gridSize) + (posX + halfGridSize);
					if (Math::Within(index, 0, (int)m_cubes.size())) {
						Pixel& pixel = m_cubes[index];
						BasicMaterial* mat = (BasicMaterial*)pixel.m_entity->m_model->GetMeshes()[0]->GetMaterial().Get();
						if (!down) downState = !pixel.m_enabled;
						pixel.m_enabled = downState;
						down = true;
						if (pixel.m_enabled) {
							mat->m_color = Color::White();
							m_inputValues[index] = 1.0;
						} else {
							mat->m_color = Color::Black();
							m_inputValues[index] = 0.0;
						}
						GetThreadPool()->DoJob([&] {m_network->feedForward(m_inputValues); });
					}
				}
			}
		} else down = false;
		if (ButtonDown(VK_MOUSE_MIDDLE)) {
			for (Pixel& pixel : m_cubes) {
				BasicMaterial* mat = (BasicMaterial*)pixel.m_entity->m_model->GetMeshes()[0]->GetMaterial().Get();
				mat->m_color = Color::White();
				pixel.m_enabled = true;
			}
			for (int i = 0; i < totalGridSize; i++) m_inputValues[i] = 1.0;
			m_network->feedForward(m_inputValues);
		}
		//for (int i = 0; i < drawableCount; i++) {
		//	if (drawableLines[i]->enabled) {
		//		DrawableLine* line = drawableLines[i];
		//		line->duration -= time.GetMills();
		//		if (line->duration <= 0) {
		//			line->enabled = false;
		//		}
		//	}
		//}
	}

	void SetPixel(int index, bool state) {
		Pixel& pixel = m_cubes[index];
		BasicMaterial* mat = (BasicMaterial*)pixel.m_entity->m_model->GetMeshes()[0]->GetMaterial().Get();
		pixel.m_enabled = state;
		if (pixel.m_enabled) {
			mat->m_color = Color::White();
			m_inputValues[index] = 1.0;
		} else {
			mat->m_color = Color::Black();
			m_inputValues[index] = 0.0;
		}
	}
	void RenderGeometry() override {
		lineRendererIndex = 0;
		m_shader->Bind();
		m_shader->Set("_Iridescence", 4);
		m_shader->Set("_IridescenceStrength", 0);
		m_shader->Set("_ProjectionMatrix", GetCamera()->GetProjectionMatrix());
		m_shader->Set("_ViewMatrix", GetCamera()->GetViewMatrix());
		m_shader->Set("_TransformationMatrix", Matrix4::Identity());
		m_shader->Set("_CameraPosition", GetCamera()->m_position);
		m_shader->Set("_TessellationFactor", (float)0);
		m_shader->Set("_TessellationSlope", (float)0);
		m_shader->Set("_TessellationShift", (float)0);
		m_shader->Set("_TessellationAlpha", (float)0);
		m_network->Draw(m_shader, renderType);
		for (auto& cube : m_cubes) cube.m_entity->Draw(m_shader, GL_PATCHES);
		for (auto& light : m_pointLights) GetPointlightRenderer()->Submit(light);

		if (Math::Within(m_rayCastPos.x, -halfGridSize - 0.5f, halfGridSize - 0.5f)) {
			if (Math::Within(m_rayCastPos.y, -halfGridSize - 0.5f, halfGridSize - 0.5f)) {
				int posX = (m_rayCastPos.x + 0.5f - (m_rayCastPos.x < 0 ? 1 : 0));
				int posY = (m_rayCastPos.y + 0.5f - (m_rayCastPos.y < 0 ? 1 : 0));
				GetLineRenderer()->DrawRectZ(Rect(posX, posY, 1, 1), Color::Red(), -0.76f);
			}
		}

		//for (int i = 0; i < drawableCount; i++) {
		//	if (drawableLines[i]->enabled) {
		//		DrawableLine* line = drawableLines[i];
		//		Color color = Color(line->amount * line->duration / 1000);
		//		color.A = 1.0;
		//		GetLineRenderer()->Submit(line->x1, line->x2, color);
		//	}
		//}

	}
	void RenderUI() override {}
	int a = 0;
	int b = 0;
	void OnImGUI() override {
		ImGui::Text("ETA - overall net learning rate\n0.0 - slow learner\n0.2 - medium learner\n1.0 - reckless learner\nalpha - momentum\n0.0 - no momentum\n0.5 - moderate momentum");
		ImGui::InputFloat("eta##1", &eta, 0.0f, 2.0f, "%.5f");
		ImGui::SliderFloat("eta##2", &eta, 0.0f, 0.1f, "%.5f");
		ImGui::SliderFloat("alpha", &alpha, 0.0f, 1.0f);
		ImGui::Checkbox("Show connection weights", &renderType);

		ImGui::Separator();

		static int iindex = 0;
		static int theNumber = 0;
		if (ImGui::SliderInt("Number from database", &iindex, 0, 60000 - 1)) {
			for (int i = 0; i < totalGridSize; i++) {
				SetPixel(i, m_numbers[iindex].data[i] == 1.0);
			}
			theNumber = m_numbers[iindex].number;
		}
		ImGui::LabelText("Number: ", "%d", theNumber);
		if (ImGui::Button("Test this input!")) {
			m_network->feedForward(m_inputValues);
		}

		vector<double> results;
		m_network->getResults(results);
		auto it = max_element(results.begin(), results.end());
		int index = distance(results.begin(), it);
		ImGui::LabelText("Guessed input:", "%d", index);

		ImGui::Separator();

		static bool training = true;
		static int progress = 0;
		static bool disabled = false;
		static float averageTimePerTraining = 0;
		static int epoch = 0;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, disabled);
		if (ImGui::Button("StartTraining")) {
			disabled = true;
			training = true;
			progress = 0;
			epoch = 0;
			GetThreadPool()->DoJob([&] {
				float avg[10] = { 0 };
				int avgIndex = 0;
				float avgSum = 0;
				while (training) {
					epoch++;
					for (int x = 0; x < 60000; x++) {
						if (!training) break;
						Timer timer;
						for (int i = 0; i < totalGridSize; i++) {
							SetPixel(i, m_numbers[x].data[i] == 1.0);
						}
						theNumber = m_numbers[x].number;
						m_network->feedForward(m_numbers[x].data);
						BackPropNumber(m_network, m_numbers[x].number);
						progress++;

						//Average
						float time = timer.Get();
						avgSum -= avg[avgIndex];
						avgSum += time;
						avg[avgIndex++] = time;
						if (avgIndex >= 10) avgIndex = 0;
						averageTimePerTraining = avgSum / 10;
					}
				}
				disabled = false;
			});

		}
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, false);
		if (ImGui::Button("StopTraining")) {
			training = false;
		}

		ImGui::LabelText("Epoch", "%d", epoch);
		ImGui::LabelText("Recent average error", "%f", m_network->m_recentAverageError);
		ImGui::LabelText("Trainings per second", "%d", (int)(1000.0f / averageTimePerTraining));

		ImGui::LabelText("Progress: ", "%d", progress);
		ImGui::ProgressBar((float)progress / 60000);

		ImGui::Separator();
		if (ImGui::Button("Export")) {
			m_network->Export();
		}
		if (ImGui::Button("Import")) {
			m_network->Import();
		}

		//if (ImGui::Button("This is a 0!")) {
		//	BackPropNumber(m_network, 0);
		//}
		//if (ImGui::Button("This is a 1!")) {
		//	BackPropNumber(m_network, 1);
		//}
		//if (ImGui::Button("This is a 2!")) {
		//	BackPropNumber(m_network, 2);
		//}
		//if (ImGui::Button("This is a 3!")) {
		//	BackPropNumber(m_network, 3);
		//}
		//if (ImGui::Button("This is a 4!")) {
		//	BackPropNumber(m_network, 4);
		//}
		//if (ImGui::Button("This is a 5!")) {
		//	BackPropNumber(m_network, 5);
		//}
		//if (ImGui::Button("This is a 6!")) {
		//	BackPropNumber(m_network, 6);
		//}
		//if (ImGui::Button("This is a 7!")) {
		//	BackPropNumber(m_network, 7);
		//}
		//if (ImGui::Button("This is a 8!")) {
		//	BackPropNumber(m_network, 8);
		//}
		//if (ImGui::Button("This is a 9!")) {
		//	BackPropNumber(m_network, 9);
		//}
		//if (ImGui::Button("Start learning")) {
		//	//for (int i = 2000; i >= 0; --i) {
		//	//	int n1 = (int)(2.0 * rand() / double(RAND_MAX));
		//	//	int n2 = (int)(2.0 * rand() / double(RAND_MAX));
		//	//	int t = n1 ^ n2;
		//	//	inputValues.push_back({ (double)n1,(double)n2 });
		//	//	outputValues.push_back({ (double)t });
		//	//}
		//	//
		//	//GetThreadPool()->DoJob([&] {
		//	//	for (int i = 0; i < inputValues.size() - 100; i++) {
		//	//		m_network->feedForward(inputValues[i]);
		//	//		vector<double> outputs;
		//	//		m_network->getResults(outputs);
		//	//		m_network->backProp(outputValues[i]);
		//	//		Sleep(2);
		//	//	}});
		//}
		//if (ImGui::SliderInt("A value", &a, 0, 1)) {
		//	m_network->feedForward({ (double)a, (double)b });
		//
		//}
		//if (ImGui::SliderInt("B value", &b, 0, 1)) {
		//	m_network->feedForward({ (double)a, (double)b });
		//
		//}
		//vector<double> outputs;
		//m_network->getResults(outputs);
		//ImGui::LabelText("Output", "%f", outputs[0]);
		//ImGui::LabelText("Expected output", "%d", a ^ b);
		//ImGui::LabelText("Correct answer?", "%s", (Math::Round(outputs[0]) == a ^ b ? "Yes!" : "No..."));
	}

	void BackPropNumber(NeuralNetwork* net, int num) {
		vector<double> result = { -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0 };
		result[num] = 1.0;
		net->backProp(result);
	}

	void BackPropTriangle(NeuralNetwork* net) {
		vector<double> result = { 0, 0, 1 };
		net->backProp(result);
	}

	void BackPropCross(NeuralNetwork* net) {
		vector<double> result = { 0, 1, 0 };
		net->backProp(result);
	}

	void BackPropCircle(NeuralNetwork* net) {
		vector<double> result = { 1, 0, 1 };
		net->backProp(result);
	}

	void Cleanup() override {}

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};