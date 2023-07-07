#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

#include "DebugText.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	// ゲーム更新描画
	void GamePlayUpdate();
	void GamePlayDraw3D();
	void GamePlayDraw2DBack();
	void GamePlayDraw2DNear();

	// ゲームタイトル更新描画
	void TitleUpdate();
	void TitleDraw2DNear();

	// ゲームオーバー更新描画
	void GameOverUpdate();
	void GameOverDraw2DNear();

private:
	// プレイヤー
	void PlayerUpdate();

	// 弾
	void BeamUpdate();
	void BeamMove();
	void BeamBorn();

	// 敵
	void EnemyUpdate();
	void EnemyMove();
	void EnemyBorn();

	// 当たり判定
	void Collision();              //衝突判定
	void CollisionPlayerEnemy();   //敵と自機
	void CollisionBeamEnemy();     //弾と敵

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>

	// BG(スプライト)
	uint32_t textureHandleBG_ = 0;
	Sprite* spriteBG_ = nullptr;

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// ステージ
	uint32_t textureHandleStage_ = 0;
	Model* modelStage_ = nullptr;
	WorldTransform worldTransformStage_;

	// プレイヤー
	uint32_t textureHandlePlayer_ = 0;
	Model* modelPlayer_ = nullptr;
	WorldTransform worldTransformPlayer_;

	// 弾
	uint32_t textureHandleBeam_ = 0;
	Model* modelBeam_ = nullptr;
	WorldTransform worldTransformBeam_;
	bool isBeamFlag_ = false;

	// 敵
	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = nullptr;
	WorldTransform worldTransformEnemy_;
	bool isEnemyFlag_ = false;

	// デバッグテキスト
	DebugText* debugText_ = nullptr;
	int gameScore_ = 0;   // ゲームスコア
	int playerLife_ = 3;    // 残機

	// シーン遷移
	int sceneMode_ = 1;

	// タイトル
	uint32_t textureHandleTitle_ = 0;
	Sprite* spriteTitle_ = nullptr;

	// ゲームオーバー
	uint32_t textureHandleGameOver_ = 0;
	Sprite* spriteGameOver_ = nullptr;

	// エンターが押されたら
	uint32_t textureHandleEnter_ = 0;
	Sprite* spriteEnter_ = nullptr;
	
	// 点滅
	int gameTimer_ = 0;
};