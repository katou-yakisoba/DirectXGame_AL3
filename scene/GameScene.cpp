#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

#include "AxisIndicator.h"
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete sprite_;

	delete model_;

	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// 読み込み処理
	textureHandle_ = TextureManager::Load("sample.png");

	// 生成
	sprite_ = Sprite::Create(textureHandle_, {10, 10});

	// モデルの画像読み込み
	modelTextureHandle_ = TextureManager::Load("sample.png");

	// 生成
	model_ = Model::Create();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// 音声の読み込み
	soundHandle_ = audio_->LoadWave("fanfare.wav");

	// 再生
	audio_->PlayWave(soundHandle_, true);

	// ライン描画
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);

	// デバッグカメラ生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 軸方向表示
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());
}

void GameScene::Update() {
	// スプライト移動
	Vector2 pos = sprite_->GetPosition();
	pos.x += 1;
	pos.y += 1;
	sprite_->SetPosition(pos);

	// 入力
	if (input_->TriggerKey(DIK_SPACE)) {
		// 音声停止
		audio_->StopWave(soundHandle_);
	}

	// デバッグテキスト
	ImGui::ShowDemoWindow();

	ImGui::Begin("Test");
	ImGui::Text("Kamata tatou %d,%d,%d", 2023, 04, 20);

	ImGui::InputFloat3("InputFloat3", inputFloat);
	ImGui::SliderFloat3("SliderFloat3", inputFloat, 0.0f, 1.0f);

	ImGui::End();

	// デバッグカメラの更新
	debugCamera_->Update();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画
	sprite_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// モデル描画
	model_->Draw(worldTransform_, debugCamera_->GetViewProjection(), modelTextureHandle_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

	// ライン描画
	PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}