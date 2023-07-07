#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

#include "MathUtilityForText.h"

#include <time.h>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete spriteBG_; // BG

	delete modelStage_; // ステージ

	delete modelPlayer_; // プレイヤー

	delete modelBeam_; // 弾

	delete modelEnemy_; // 敵

	delete spriteTitle_; // タイトル

	delete spriteGameOver_; // ゲームオーバー

	delete spriteEnter_; // スペース
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// BG(2Dスプライト)
	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});

	// ビュープロジェクション
	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();

	// ステージ
	textureHandleStage_ = TextureManager::Load("stage.jpg");
	modelStage_ = Model::Create();
	worldTransformStage_.Initialize();

	// ステージの位置を更新
	worldTransformStage_.translation_ = {0, -1.5f, 0};
	worldTransformStage_.scale_ = {4.5f, 1, 40};

	// 変換行列を更新
	worldTransformStage_.matWorld_ = MakeAffineMatrix(
	    worldTransformStage_.scale_, worldTransformStage_.rotation_,
	    worldTransformStage_.translation_);

	// 変換行列を定数バッファに転送
	worldTransformStage_.TransferMatrix();

	// プレイヤー
	textureHandlePlayer_ = TextureManager::Load("Player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();

	// 弾
	textureHandleBeam_ = TextureManager::Load("Beam.png");
	modelBeam_ = Model::Create();
	worldTransformBeam_.scale_ = {0.2f, 0.2f, 0.2f};
	worldTransformBeam_.Initialize();

	// 敵
	textureHandleEnemy_ = TextureManager::Load("Enemy.png");
	modelEnemy_ = Model::Create();
	worldTransformEnemy_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformEnemy_.Initialize();

	// 乱数
	srand((unsigned int)time(NULL));

	// デバッグテキスト
	debugText_ = DebugText::GetInstance();
	debugText_->Initialize();

	// タイトル
	textureHandleTitle_ = TextureManager::Load("title.png");
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {0, 0});

	// ゲームオーバー
	textureHandleGameOver_ = TextureManager::Load("gameover.png");
	spriteGameOver_ = Sprite::Create(textureHandleGameOver_, {0, 0});

	// エンター
	textureHandleEnter_ = TextureManager::Load("enter.png");
	spriteEnter_ = Sprite::Create(textureHandleEnter_, {0, 0});
}

void GameScene::Update() 
{
	switch (sceneMode_) {
	case 0:
		GamePlayUpdate();
		break;
	
	case 1:
		TitleUpdate();
		break;

	case 2:
		GameOverUpdate();
		break;
	}
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
	GamePlayDraw2DBack();
	
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
	GamePlayDraw3D();


	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	switch (sceneMode_)
	{
	case 0:

		GamePlayDraw2DNear();

		break;

	case 1:

		TitleDraw2DNear();

		break;

	case 2:

		GameOverDraw2DNear();

		break;
	}
	
	// スプライト描画後処理
	Sprite::PostDraw();
	  
#pragma endregion
}

void GameScene::PlayerUpdate() {
	// 移動

	// 右へ移動
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransformPlayer_.translation_.x += 0.1f;
	}

	// 左へ移動
	if (input_->PushKey(DIK_LEFT)) {
		worldTransformPlayer_.translation_.x -= 0.1f;
	}

	// 範囲制限

	worldTransformPlayer_.translation_.x = max(worldTransformPlayer_.translation_.x, -4);
	worldTransformPlayer_.translation_.x = min(worldTransformPlayer_.translation_.x, 4);

	// 変換行列を更新
	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
	    worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_,
	    worldTransformPlayer_.translation_);
	// 変換行列を定数バッファに転送
	worldTransformPlayer_.TransferMatrix();
}

void GameScene::BeamUpdate() {
	BeamMove();
	BeamBorn();

	// 変換行列を更新
	worldTransformBeam_.matWorld_ = MakeAffineMatrix(
	    worldTransformBeam_.scale_, worldTransformBeam_.rotation_,
	    worldTransformBeam_.translation_);
	// 変換行列を定数バッファに転送
	worldTransformBeam_.TransferMatrix();
}

void GameScene::BeamMove() {
	if (isBeamFlag_ == true) {
		worldTransformBeam_.translation_.z += 0.5f;
		worldTransformBeam_.rotation_.x += 0.1f;

		// 画面外の処理
		if (worldTransformBeam_.translation_.z >= 40.f) {
			isBeamFlag_ = false;
		}
	}
}

void GameScene::BeamBorn() {
	if (input_->TriggerKey(DIK_SPACE) && isBeamFlag_ == false) {
		worldTransformBeam_.translation_.x = worldTransformPlayer_.translation_.x;
		worldTransformBeam_.translation_.z = worldTransformPlayer_.translation_.z;

		isBeamFlag_ = true;
	}
}

void GameScene::EnemyUpdate() {
	EnemyMove();
	EnemyBorn();

	// 変換行列を更新
	worldTransformEnemy_.matWorld_ = MakeAffineMatrix(
	    worldTransformEnemy_.scale_, worldTransformEnemy_.rotation_,
	    worldTransformEnemy_.translation_);
	// 変換行列を定数バッファに転送
	worldTransformEnemy_.TransferMatrix();
}

void GameScene::EnemyMove() {
	if (isEnemyFlag_ == true) {
		worldTransformEnemy_.translation_.z -= 0.5f;
		worldTransformEnemy_.rotation_.x -= 0.1f;

		if (worldTransformEnemy_.translation_.z < -5.f) {
			isEnemyFlag_ = false;
		}
	}
}

void GameScene::EnemyBorn() {
	if (isEnemyFlag_ == false) {
		isEnemyFlag_ = true;

		worldTransformEnemy_.translation_.z = 40;

		int x = rand() % 80;
		float x2 = (float)x / 10 - 4;
		worldTransformEnemy_.translation_.x = x2;
	}
}

void GameScene::Collision() {
	CollisionPlayerEnemy();

	CollisionBeamEnemy();
}

void GameScene::CollisionPlayerEnemy() {
	// 敵が存在していれば
	if (isEnemyFlag_ == true) {
		float dx = abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_.translation_.x);
		float dz = abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_.translation_.z);

		if (dx < 1 && dz < 1) {
			isEnemyFlag_ = false;

			playerLife_--;
		}
	}
}

void GameScene::CollisionBeamEnemy() {
	// 弾が撃たれていれば
	if (isBeamFlag_ == true) {
		float bx = abs(worldTransformBeam_.translation_.x - worldTransformEnemy_.translation_.x);
		float bz = abs(worldTransformBeam_.translation_.z - worldTransformEnemy_.translation_.z);

		if (bx < 1 && bz < 1) {
			isEnemyFlag_ = false;
			isBeamFlag_ = false;
			gameScore_++;
		}
	}
}

// sceneMode_が0の場合行われる処理
void GameScene::GamePlayUpdate() {
	if (playerLife_ <= 0) {
		sceneMode_ = 2;

		playerLife_ = 3;

		gameScore_ = 0;
	}

	PlayerUpdate();

	BeamUpdate();

	EnemyUpdate();

	Collision();
}

void GameScene::GamePlayDraw3D() {
	// ステージ
	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandleStage_);

	// プレイヤー
	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);

	// 弾
	if (isBeamFlag_ == true) {
		modelBeam_->Draw(worldTransformBeam_, viewProjection_, textureHandleBeam_);
	}

	// 敵
	if (isEnemyFlag_ == true) {
		modelEnemy_->Draw(worldTransformEnemy_, viewProjection_, textureHandleEnemy_);
	}
}

void GameScene::GamePlayDraw2DBack() {
	// 背景
	spriteBG_->Draw();
}

void GameScene::GamePlayDraw2DNear() {
	char str[100];
	sprintf_s(str, "SCORE %d", gameScore_);
	debugText_->Print(str, 10, 10, 2);

	sprintf_s(str, "LIFE %d", playerLife_);
	debugText_->Print(str, 190, 10, 2);

	debugText_->DrawAll();
}

// sceneMode_が1の場合行われる処理
void GameScene::TitleUpdate()
{
	if (input_->TriggerKey(DIK_RETURN))
	{
		sceneMode_ = 0;
	}
}

void GameScene::TitleDraw2DNear()
{
	spriteTitle_->Draw();
	
	spriteEnter_->Draw();
}

// sceneMode_が2の場合行われる処理
void GameScene::GameOverUpdate()
{
	if (input_->TriggerKey(DIK_RETURN))
	{
		sceneMode_ = 0;
	}
}

void GameScene::GameOverDraw2DNear() 
{
	spriteGameOver_->Draw();

	spriteEnter_->Draw();
}