#include "getball.h"
#include "utils/maths.h"

//�û�ע�⣻�ӿ���Ҫ��������
extern "C"_declspec(dllexport) PlayerTask player_plan(const WorldModel* model, int robot_id);

enum ball_near  //PenaltyArea
{

	outOfOrbit,
	onOrbit,
	shoot
};

PlayerTask player_plan(const WorldModel* model, int robot_id){
	PlayerTask task;
	//��ֵ��С��0�����11���������ԣ�Ϊ�˷�����Чֵ�����ܸ�0��5��������2��3̨��ʱ�и����������غ�
	int robot1= -1, robot2= -1, receiver= -1;
	for (int i = 0; i < 6; i++){
		if (i == robot_id || i == model->get_our_goalie())
			continue;
		if (model->get_our_exist_id()[i])
			robot1 = i;
	}
	for (int i = 0; i < 6; i++){
		if (i == robot1 || i == robot_id || i == model->get_our_goalie())
			continue;
		if (model->get_our_exist_id()[i])
			robot2 = i;
	}
	//��ȡ�ҷ���Ա����
	const point2f& robot1_pos = model->get_our_player_pos(robot1);
	const point2f& robot2_pos = model->get_our_player_pos(robot2);
	//��ȡ�ҷ���Ա���ҷ����ŵľ���
	const point2f& goal = FieldPoint::Goal_Center_Point;
	float robot1_dist = (robot1_pos - goal).length();
	float robot2_dist = (robot2_pos - goal).length();
	if (robot1_dist < robot2_dist)
		receiver = robot1;
	else
		receiver = robot2;
	//��ȡ�ҷ���Ա����
	const point2f& receiver_pos = model->get_our_player_pos(receiver);
	const point2f& kicker = model->get_our_player_pos(robot_id);
	point2f opp_goal = receiver_pos;

	const float& circleR = 30;
	const float& DetAngle = 0.6;
	//��ȡ�������
	const point2f& ball = model->get_ball_pos();
	const float& dir = model->get_our_player_dir(robot_id);
	ball_near orbit;
	point2f shootPosOnOrbit = ball + Maths::vector2polar(circleR, (ball - opp_goal).angle());
	float toShootDir = fabs((kicker - ball).angle() - (ball - opp_goal).angle());
	//��ȡ���ֵ���ľ���
	float toBallDist = (kicker - ball).length();
	float toOppGoalDir = (opp_goal - kicker).angle();
	float toBallDir = (ball - kicker).angle();
	point2f robotBallAcrossCirclePoint = ball + Maths::vector2polar(circleR, (kicker - ball).angle());
	point2f AntishootPosOnOrbit = ball + Maths::vector2polar(circleR, (opp_goal - ball).angle());
	point2f BallToRobot = kicker - ball;


	if (toBallDist >circleR + 10)
		orbit = outOfOrbit;
	else if (toShootDir > 1)
		orbit = onOrbit;
	else
		orbit = shoot;

	bool getBall = toBallDist < 10;
	float diffdir_onorbit = 0;
	float b2r = BallToRobot.angle();
	float o2b = (ball - opp_goal).angle();
	bool add;
	switch (orbit)
	{
	case outOfOrbit:
		task.target_pos = robotBallAcrossCirclePoint;
		task.orientate = toOppGoalDir;
		break;
	case onOrbit:
		if (b2r * o2b >0){
			if (b2r > 0){
				if (b2r > o2b)
					add = false;
				else
					add = true;
			}
			else{
				if (b2r > o2b)
					add = false;
				else
					add = true;
			}
		}
		else{
			if (b2r > 0)
				add = true;
			else
				add = false;
		}
		if (add)
		{
			//+
			task.target_pos = ball + Maths::vector2polar(circleR, BallToRobot.angle() + DetAngle);
			task.orientate = toOppGoalDir;
		}
		else
		{
			//-
			task.target_pos = ball + Maths::vector2polar(circleR, BallToRobot.angle() - DetAngle);
			task.orientate = toOppGoalDir;
		}
		break;
	case shoot:
		task.target_pos = ball + Maths::vector2polar(12, (ball - opp_goal).angle());
		task.orientate = toOppGoalDir;
		break;
	}
	return task;
}