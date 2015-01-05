#ifndef MY_TRANS_SIMU_H
#define MY_TRANS_SIMU_H

#include <cmath>
#include <ctime>
#include <vector>
#include <stack>
using namespace std;

// 常量
#define PI					3.141592
#define LAYER_NUM			4	// CreateCrossRoad 的图层数目，-> 类的m_lyrNum
#define CAR_COLOR_NUM		5	
#define LIGHT_COLOR_NUM		3
#define ANGLE_NUM			360

// 存储数据
static double LIGHT[4];							// 红绿灯的四个标识
static double SIDEWALK[4];						// 人行道标识

static COLORREF CarColor[CAR_COLOR_NUM] =
{ RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255), RGB(255, 255, 0), 
RGB(255, 0, 255) };

static COLORREF LightColor[LIGHT_COLOR_NUM] =
{ RGB(255, 0, 0), RGB(0, 255, 0), RGB(255, 255, 0) };

// 结构体
struct Pos
{
	double x;
	double y;
};
struct OptionSet
{
	// 道路参数
	float rd_wid;			
	float rd_xlen, rd_ylen;
	float rd_sidwk_wid;
	float rd_cen_wid;

	int rd_lnm;
	int rd_rnm;
	float rd_arc;

	// 车参数
	double car_speed;
	int car_size;

	// 行人参数
	double per_speed;
	int per_size;

	// 投放周期
	UINT car_elapse_circle;		// ms
	UINT per_elapse_circle;		// ms

	// 投放模式
	int generate_num;			// 0 随机投放 1 随机投放

	// 颜色设置
	COLORREF bak_clr;
	COLORREF per_clr;

	// 其它参数
	UINT light_circle;
	UINT light_ref;
	UINT car_ref;
	UINT per_ref;
	float car_dis;
	float per_light_dis;
	float buf;
	int light_size;
};

struct Stastics
{
	int totalCar;
	int totalPer;
	int totalEvtNum;
	UINT time_run;
	UINT time_left;
};
// 类
class TrafficLight
{
public:
	int evntNum;		// evnt 编号，唯一
	TrafficLight();
	~TrafficLight();
	void CreateLight(Pos pt1, Pos pt2);
	void CreateLight2(Pos pt1, Pos pt2, Pos pt3, Pos pt4);
	void ChangeLightColor(short id = 0);	// 设置灯的颜色 id：0-红 1-绿 2-黄

private:
	CMoGeoEvent evnt;	// 事件
};

class Car
{
public:
	Car();
	void CreateCar(int lNum = 1, double dis = 1);
	~Car();
	CMoGeoEvent evnt;	// 事件

private:
	// 成员变量
	int ptsInd;			// 点集的索引号
	int symColorInd;	// 颜色 Index
	double stepDis;		// 每个周期行驶的距离
	int divNum;			// 行驶的步数

public:
	CMoPoints pts;		// 轨迹离散点集
	int evntNum;		// evnt 编号，唯一
	BOOL InOneWay(int l1, int l2);		// 是否在同一跑道上
	int lneNum;				// 路线编号
	double initAng;			// 初始角度
	BOOL flagState;
	BOOL flagBump;			// 停止标志（立即停止）
	// 函数
	void Move();		// 移动
	void Disappear();	// 消失
	void Bump();		// 碰撞检测
	BOOL IsInCenterRegion(Pos pt);
	BOOL IsInArcRoad();
};

class Person
{
public:
	Person();
	~Person();
	void CreatePerson(int lNum, double dis = 0.2, double len = 0.4);
	double RandGenerate(int a = 0, int b = 1);
	void GeneratePts(CMoPoint& pt1, CMoPoint& pt2);
	void Move();
	void Disappear();
	void Bump();

	CMoGeoEvent evnt;
	int evntNum;		// evnt 编号，唯一
	int lneNum;			// 路线编号
	CMoPoints pts;
	BOOL flagState;
	BOOL flagBump;

	double stepDis;

private:
	int divNum;
	int ptsInd;
};

static CMoTrackingLayer MoTrackingLayer;
static vector<CMoLine> MoTrackLine;
static vector<TrafficLight> m_Light;
static vector<Car> m_Car;
static vector<Person> m_Per;
static OptionSet m_OptSet;
static Stastics m_Sta;

#endif