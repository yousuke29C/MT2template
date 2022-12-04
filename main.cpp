#include <DxLib.h>
#include "Vector3.h"
#include <cstring> //memcpy

//線分の描画
//Dxlib => int DrawLine3D(VECTOR Pos1,VECTOR Pos2,const unsigned int Color);
int DrawLine3D(const Vector3& Pos1, const Vector3& Pos2, const unsigned int Color);

//カメラの位置と姿勢の設定
//Dxlib => int SetCameraPositionAndTargetAndUpVec(VECTOR Position, VECTOR Target, VECTOR Up);
int SetCameraPositionAndTargetAndUpVec(
	const Vector3& cameraPosition,	//カメラの位置
	const Vector3& cameraTarget,	//カメラの注視点
	const Vector3& cameraUp			//カメラの上の向き
);

//球の描画
//Dxlib => int DrawSphere3D(VECTOR CenterPos,float r,int DivNum,unsigned int DifColor,unsigned int SpcColor,int FillFlag);
int DrawSphere3D(const Vector3& CenterPos, const float r, const int DivNum,
	const unsigned int DifColor, const unsigned int SpcColor, const int FillFlag);

//関数プロトタイプ宣言
void DrawAxis3D(const float length);	//x,y,z軸の描画

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {

	const int WindowWidth = 1024;
	const int WindowHeight = 576;

	ChangeWindowMode(true);//ウィンドウモードになる
	SetGraphMode(WindowWidth, WindowHeight, 32);//画面モードのセット
	SetBackgroundColor(0, 0, 64);//背景色RGB
	if (DxLib_Init() == -1) { return -1; }// DXlibの初期化
	SetDrawScreen(DX_SCREEN_BACK);// (ダブルバッファ)描画先グラフィック領域は裏面を指定

	// Ｚバッファを有効にする
	SetUseZBuffer3D(TRUE);
	// Ｚバッファへの書き込みを有効にする
	SetWriteZBuffer3D(TRUE);

	////カメラの初期化
	//Vector3 cameraPosition(50.0f, 50.0f, -400.0f);
	//Vector3 cameraTarget(0.0f, 0.0f, 0.0f);
	//Vector3 cameraUp(0.0f, 1.0f, 0.0f);

	//クリップ面        近　　　 遠
	SetCameraNearFar(1.0f, 1000.0f);//カメラの有効範囲の設定
	SetCameraScreenCenter(WindowWidth / 2.0f, WindowHeight / 2.0f);//画面の中心をカメラの中心に合わせる
	SetCameraPositionAndTargetAndUpVec(
		Vector3(0.0f, 0.0f, -120.0f),			//カメラの位置
		//Vector3(-20.0f,20.0f,-120.0f),		//カメラの位置
		Vector3(0.0f, 0.0f, 0.0f),				//カメラの注視点
		Vector3(0.0f, 1.0f, 0.0f)				//カメラの上の向き
	);

	//時間計算に必要なデータ
	long long startCount = 0;
	long long nowCount = 0;
	long long elapsedCount = 0;

	//補間で使うデータ
	//start → end を5秒で完了させる
	Vector3 start(-100.0f, 0, 0);	//スタート地点
	Vector3 end(100.0f, 0, 0);		//エンド地点
	float maxTime = 5.0f;			//全体時間[s]
	float timeRate;					//何％時間が進んだか

	//球の位置
	Vector3 position;

	//実行前にカウント値を取得
	startCount = GetNowHiPerformanceCount(); //long long int型 64bit int

	//ゲームループ
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {

		//// 更新処理

		//[R]でリセット
		if (CheckHitKey(KEY_INPUT_R)) {
			startCount = GetNowHiPerformanceCount();
		}

		//経過時間(elapsedTime[s])の計算
		nowCount = GetNowHiPerformanceCount();
		elapsedCount = nowCount - startCount;
		float elapsedTime = static_cast<float> (elapsedCount) / 1000000.0f;

		//スタート地点		: start
		//エンド地点		: end
		//経過時間		: elapsed[s]
		//移動完了の率	(経過時間/全体時間) : timeRate(％)

		timeRate = min(elapsedTime / maxTime, 1.0f);

		position = lerp(start, end, timeRate);
		//position = easeIn(start,end,timerate);
		//position = easeOut(start,end,timerate);
		//position = easeInOut(start,end,timerate);

		// 描画処理
		ClearDrawScreen();	//画面を消去
		DrawAxis3D(500.0f);	//xyz軸の描画

		//球の描画
		DrawSphere3D(position, 5.0f, 32, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);

		DrawFormatString(0, 0, GetColor(255, 255, 255), "position (%5.1f,%5.1f,%5.1f)", position.x, position.y, position.z);
		DrawFormatString(0, 20, GetColor(255, 255, 255), "%7.3f[s]", elapsedTime);
		DrawFormatString(0, 40, GetColor(255, 255, 255), "[R]: Restart");
		//フリップする
		ScreenFlip();
	}

	//20ミリ秒待機(疑似60FPS)
	WaitTimer(20);

	// Dxライブラリ終了処理
	DxLib_End();

	// 正常終了
	return 0;
}

//x,y,z軸の描画
void DrawAxis3D(const float length) {
	//軸の線の描画
	DrawLine3D(Vector3(-length, 0, 0), Vector3(+length, 0, 0), GetColor(255, 0, 0));
	DrawLine3D(Vector3(0, -length, 0), Vector3(0, +length, 0), GetColor(0, 255, 0));
	DrawLine3D(Vector3(0, 0, -length), Vector3(0, 0, +length), GetColor(0, 0, 255));
}

//キー操作の描画
void DrawKeyOperation() {
	const unsigned white = GetColor(255, 255, 255);

	DrawFormatString(10, 20 * 1, white, "[W][E][R]  R : リセット");
	DrawFormatString(10, 20 * 2, white, "[A][S][D] AD : y軸まわりの回転");
	DrawFormatString(10, 20 * 3, white, "[Z]	   WS : X軸まわりの回転");
	DrawFormatString(10, 20 * 4, white, "		　　EZ : Z軸まわりの回転");
}

//以降、Dxlibの各関数でVector3型 Matrix4型 を利用できるようにする関数群
//球の描画
//Dxlib => int DrawSphere3D(VECTOR CenterPos, float r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag);

int DrawSphere3D(const Vector3& CenterPos, const float r, const int DivNum,
	const unsigned int DifColor, const unsigned int SpcColor, const int FillFlag) {
	VECTOR centerPos = { CenterPos.x,CenterPos.y,CenterPos.z };

	return DrawSphere3D(centerPos, r, DivNum, DifColor, SpcColor, FillFlag);
}

//線分の描画
//Dxlib => int DrawLine3D(VECTOR Pos1, VECTOR Pos2, unsigned int Color);
int DrawLine3D(const Vector3& Pos1, const Vector3& Pos2, const unsigned int Color) {
	VECTOR p1 = { Pos1.x, Pos1.y, Pos1.z };
	VECTOR p2 = { Pos2.x, Pos2.y, Pos2.z };

	return DrawLine3D(p1, p2, Color);
}

// カメラの位置と姿勢の設定
int SetCameraPositionAndTargetAndUpVec(
	const Vector3& cameraPosition,  // カメラの位置
	const Vector3& cameraTarget,    // カメラの注視点
	const Vector3& cameraUp         // カメラの上の向き
) {
	VECTOR position = { cameraPosition.x,cameraPosition.y,cameraPosition.z };
	VECTOR target = { cameraTarget.x,cameraTarget.y,cameraTarget.z };
	VECTOR up = { cameraUp.x,cameraUp.y,cameraUp.z };

	return SetCameraPositionAndTargetAndUpVec(position, target, up);
}