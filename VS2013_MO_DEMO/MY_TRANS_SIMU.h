#ifndef MY_TRANS_SIMU_H
#define MY_TRANS_SIMU_H

#include <cmath>
#include <ctime>
#include <vector>
#include <stack>
using namespace std;

// ����
#define PI					3.141592
#define LAYER_NUM			4	// CreateCrossRoad ��ͼ����Ŀ��-> ���m_lyrNum
#define CAR_COLOR_NUM		5	
#define LIGHT_COLOR_NUM		3
#define ANGLE_NUM			360

// �洢����
static double LIGHT[4];							// ���̵Ƶ��ĸ���ʶ
static double SIDEWALK[4];						// ���е���ʶ

static COLORREF CarColor[CAR_COLOR_NUM] =
{ RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255), RGB(255, 255, 0), 
RGB(255, 0, 255) };

static COLORREF LightColor[LIGHT_COLOR_NUM] =
{ RGB(255, 0, 0), RGB(0, 255, 0), RGB(255, 255, 0) };

// �ṹ��
struct Pos
{
	double x;
	double y;
};
struct OptionSet
{
	// ��·����
	float rd_wid;			
	float rd_xlen, rd_ylen;
	float rd_sidwk_wid;
	float rd_cen_wid;

	int rd_lnm;
	int rd_rnm;
	float rd_arc;

	// ������
	double car_speed;
	int car_size;

	// ���˲���
	double per_speed;
	int per_size;

	// Ͷ������
	UINT car_elapse_circle;		// ms
	UINT per_elapse_circle;		// ms

	// Ͷ��ģʽ
	int generate_num;			// 0 ���Ͷ�� 1 ���Ͷ��

	// ��ɫ����
	COLORREF bak_clr;
	COLORREF per_clr;

	// ��������
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
// ��
class TrafficLight
{
public:
	int evntNum;		// evnt ��ţ�Ψһ
	TrafficLight();
	~TrafficLight();
	void CreateLight(Pos pt1, Pos pt2);
	void CreateLight2(Pos pt1, Pos pt2, Pos pt3, Pos pt4);
	void ChangeLightColor(short id = 0);	// ���õƵ���ɫ id��0-�� 1-�� 2-��

private:
	CMoGeoEvent evnt;	// �¼�
};

class Car
{
public:
	Car();
	void CreateCar(int lNum = 1, double dis = 1);
	~Car();
	CMoGeoEvent evnt;	// �¼�

private:
	// ��Ա����
	int ptsInd;			// �㼯��������
	int symColorInd;	// ��ɫ Index
	double stepDis;		// ÿ��������ʻ�ľ���
	int divNum;			// ��ʻ�Ĳ���

public:
	CMoPoints pts;		// �켣��ɢ�㼯
	int evntNum;		// evnt ��ţ�Ψһ
	BOOL InOneWay(int l1, int l2);		// �Ƿ���ͬһ�ܵ���
	int lneNum;				// ·�߱��
	double initAng;			// ��ʼ�Ƕ�
	BOOL flagState;
	BOOL flagBump;			// ֹͣ��־������ֹͣ��
	// ����
	void Move();		// �ƶ�
	void Disappear();	// ��ʧ
	void Bump();		// ��ײ���
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
	int evntNum;		// evnt ��ţ�Ψһ
	int lneNum;			// ·�߱��
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