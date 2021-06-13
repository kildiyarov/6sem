#include <GL/glut.h>//для отрисовки
#include <time.h>//для таймера
#include <random>//для случайных чисел
#include <iostream> // ввод - вывод
#include <conio.h>//для управления с клавы
#include <limits>//Для инициализации float min
//#include <windows.h>

#define  N 4
#define  M 4 //размер поля NхM квадратов
#define players_num 2// Количество игроков
int scale = 50; // размер квадрата
int w = scale * N; // ширина поля  
int h = scale * M; // его высота


//Данные игроков и бомб:
int dir1=0, bumb1 = 0; // 4 направления и индикатор сброса бомбы 1го игрока.
int dir2 = 0, bumb2 = 0; // 4 направления и индикатор сброса бомбы 2го игрока.
int boombs = 0;//Количество бомб
int MAX_bombs = 2;//максимальное к-во бомб на карте
int wins1 = 0, wins2 = 0;//кол-во выигрышей 1го и 2го игроков вообще
int winsy1 = 0, winsy2 = 0;//к-во побед 1го и 2го агента за 1000 последних игр

//Состояние мира:
float players_and_walls[N][M] = { 0 };//-1 - стенка, 1 - 1 игрок, 2 - 2 игрок,
float booms[N][M] = { 0 };// <=-2 - огонь, -3..-4 - бомба, значение зависит от ее готовности взорваться
//Функция начальной генерации мира
void Start_generation() {
	//Рисуем простейшую карту
	players_and_walls[0][0] = 1, players_and_walls[1][0] = 0, players_and_walls[2][0] = 0, players_and_walls[3][0] = 0;
	players_and_walls[0][1] = -1, players_and_walls[1][1] = 0, players_and_walls[2][1] = -1, players_and_walls[3][1] = 0;
	players_and_walls[0][2] = 0, players_and_walls[1][2] = 0, players_and_walls[2][2] = 0, players_and_walls[3][2] = 0;
	players_and_walls[0][3] = -1, players_and_walls[1][3] = 0, players_and_walls[2][3] = -1, players_and_walls[3][3] = 2;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			booms[i][j] = 0;
		}
	}
	dir1 = 0, bumb1 = 0; // 4 направления и индикатор сброса бомбы 1го игрока.
	dir2 = 0, bumb2 = 0; // 4 направления и индикатор сброса бомбы 2го игрока.
	boombs = 0;//Количество бомб
}



/*Матрица вероятностей: Положение игрока, положение врага, положения 1й бомбы, 
 положения 2й бомбы, варианты действий (0- ничего, 1 - класть бомбу, 2...5 за движение)*/
float strategy11[13][13][13][13][13][13][6] = { 0 };
float strategy12[13][13][13][13][13][13][6] = { 0 };

float strategy21[13][13][13][13][13][13][6] = { 0 };
float strategy22[13][13][13][13][13][13][6] = { 0 };

//заполняет матрицу стратегии случайными числами или нулями
void random_strategy() {
	for (int i1 = 0; i1 < 13; i1++) {
		for (int i2 = 0; i2 < 13; i2++) {
			for (int i3 = 0; i3 < 13; i3++) {
				for (int i4 = 0; i4 < 13; i4++) {
					for (int i5 = 0; i5 < 13; i5++) {
						for (int i6 = 0; i6 < 13; i6++) {
							for (int i7 = 0; i7 < 6; i7++) {
								strategy11[i1][i2][i3][i4][i5][i6][i7] = ((float)rand() / (float)RAND_MAX);
								strategy12[i1][i2][i3][i4][i5][i6][i7] = ((float)rand() / (float)RAND_MAX);
								strategy21[i1][i2][i3][i4][i5][i6][i7] = ((float)rand() / (float)RAND_MAX);
								strategy22[i1][i2][i3][i4][i5][i6][i7] = ((float)rand() / (float)RAND_MAX);
							}
						}
					}
				}
			}
		}
	}
}
void null_strategy() {
	for (int i1 = 0; i1 < 13; i1++) {
		for (int i2 = 0; i2 < 13; i2++) {
			for (int i3 = 0; i3 < 13; i3++) {
				for (int i4 = 0; i4 < 13; i4++) {
					for (int i5 = 0; i5 < 13; i5++) {
						for (int i6 = 0; i6 < 13; i6++) {
							for (int i7 = 0; i7 < 6; i7++) {
								strategy11[i1][i2][i3][i4][i5][i6][i7] = 0.0;
								strategy12[i1][i2][i3][i4][i5][i6][i7] = 0.0;
								strategy21[i1][i2][i3][i4][i5][i6][i7] = 0.0;
								strategy22[i1][i2][i3][i4][i5][i6][i7] = 0.0;
							}
						}
					}
				}
			}
		}
	}
}

FILE* stream1;
FILE* stream2;
void save_strategy1(char name[]) {
	fopen_s(&stream1, name, "w");
	for (int i1 = 0; i1 < 13; i1++) {
		for (int i2 = 0; i2 < 13; i2++) {
			for (int i3 = 0; i3 < 13; i3++) {
				for (int i4 = 0; i4 < 13; i4++) {
					for (int i5 = 0; i5 < 13; i5++) {
						for (int i6 = 0; i6 < 13; i6++) {
							fprintf(stream1, "%f %f %f %f %f %f\n",
								strategy11[i1][i2][i3][i4][i5][i6][0],
								strategy11[i1][i2][i3][i4][i5][i6][1],
								strategy11[i1][i2][i3][i4][i5][i6][2],
								strategy11[i1][i2][i3][i4][i5][i6][3],
								strategy11[i1][i2][i3][i4][i5][i6][4],
								strategy11[i1][i2][i3][i4][i5][i6][5]);
						}
					}
				}
			}
		}
	}
	fclose(stream1);
}
void save_strategy2(char name[]) {
	fopen_s(&stream2, name, "w");
	for (int i1 = 0; i1 < 13; i1++) {
		for (int i2 = 0; i2 < 13; i2++) {
			for (int i3 = 0; i3 < 13; i3++) {
				for (int i4 = 0; i4 < 13; i4++) {
					for (int i5 = 0; i5 < 13; i5++) {
						for (int i6 = 0; i6 < 13; i6++) {
							fprintf(stream1, "%f %f %f %f %f %f\n",
								strategy12[i1][i2][i3][i4][i5][i6][0],
								strategy12[i1][i2][i3][i4][i5][i6][1],
								strategy12[i1][i2][i3][i4][i5][i6][2],
								strategy12[i1][i2][i3][i4][i5][i6][3],
								strategy12[i1][i2][i3][i4][i5][i6][4],
								strategy12[i1][i2][i3][i4][i5][i6][5]);
						}
					}
				}
			}
		}
	}
	fclose(stream2);
}
void load_strategy1(char name[]) {
	fopen_s(&stream1, name, "r");
	for (int i1 = 0; i1 < 13; i1++) {
		for (int i2 = 0; i2 < 13; i2++) {
			for (int i3 = 0; i3 < 13; i3++) {
				for (int i4 = 0; i4 < 13; i4++) {
					for (int i5 = 0; i5 < 13; i5++) {
						for (int i6 = 0; i6 < 13; i6++) {
							fscanf_s(stream1, "%f %f %f %f %f %f\n",
								&strategy11[i1][i2][i3][i4][i5][i6][0],
								&strategy11[i1][i2][i3][i4][i5][i6][1],
								&strategy11[i1][i2][i3][i4][i5][i6][2],
								&strategy11[i1][i2][i3][i4][i5][i6][3],
								&strategy11[i1][i2][i3][i4][i5][i6][4],
								&strategy11[i1][i2][i3][i4][i5][i6][5]);
						}
					}
				}
			}
		}
	}
	fclose(stream1);
}
void load_strategy2(char name[]) {
	fopen_s(&stream2, name, "r");
	for (int i1 = 0; i1 < 13; i1++) {
		for (int i2 = 0; i2 < 13; i2++) {
			for (int i3 = 0; i3 < 13; i3++) {
				for (int i4 = 0; i4 < 13; i4++) {
					for (int i5 = 0; i5 < 13; i5++) {
						for (int i6 = 0; i6 < 13; i6++) {
							fscanf_s(stream2, "%f %f %f %f %f %f\n",
								&strategy12[i1][i2][i3][i4][i5][i6][0],
								&strategy12[i1][i2][i3][i4][i5][i6][1],
								&strategy12[i1][i2][i3][i4][i5][i6][2],
								&strategy12[i1][i2][i3][i4][i5][i6][3],
								&strategy12[i1][i2][i3][i4][i5][i6][4],
								&strategy12[i1][i2][i3][i4][i5][i6][5]);
						}
					}
				}
			}
		}
	}
	fclose(stream2);
}


/*ищем по текущей ситуации лучшее движение для 1го игрока с вероятностью случайного движения, 
  и записываем это состояние и вероятности в массивы возвращая номер действия*/
int Q_player1(int indexes[], float rewards[], float random) {
	//случайные действия
	if (((float)rand() / (float)RAND_MAX) < random) {
		int randomishe = rand() % 5+1;
		if (randomishe == 0) bumb1 = 0, dir1 = 0;
		if (randomishe == 1) bumb1 = 1;
		if (randomishe == 2) dir1 = 1;
		if (randomishe == 3) dir1 = 2;
		if (randomishe == 4) dir1 = 3;
		if (randomishe == 5) dir1 = 4;
		return randomishe;
	}

	//Не случайные действия
	int index[12][2] = { {1,3},{3,3},//координаты ячеек где нет стен - пространство агента
						{0,2},{1,2},{2,2},{3,2},
						{1,1},{3,1},
						{0,0},{1,0},{2,0},{3,0} };
	int player = 0;//По-умолчанию на карте нет ничего
	int enemy = 0;
	int bomb11 = 0;
	int bomb12 = 0;
	int bomb21 = 0;
	int bomb22 = 0;

	for (int i = 0; i < 12; i++) {
		if (players_and_walls[index[i][0]][index[i][1]] == 1) player = i+1;//i+1 т.к. в стратегии нуль означает отсутствие
		if (players_and_walls[index[i][0]][index[i][1]] == 2) enemy = i+1;
		if (booms[index[i][0]][index[i][1]] >= 1 && booms[index[i][0]][index[i][1]] <= 1.5 && bomb11 == 0) bomb11 = i+1;
		if (booms[index[i][0]][index[i][1]] > 1.5 && booms[index[i][0]][index[i][1]] <= 2  && bomb12 == 0) bomb12 = i+1;
		if (booms[index[i][0]][index[i][1]] >= 1 && booms[index[i][0]][index[i][1]] <= 1.5 && bomb11 != 0) bomb21 = i+1;
		if (booms[index[i][0]][index[i][1]] > 1.5 && booms[index[i][0]][index[i][1]] <= 2 && bomb12 != 0) bomb22 = i+1;
	}
	int action = 0;//По-умолчанию мы ничего не делаем
	float max= std::numeric_limits<float>::min();
	for (int i = 1; i < 6; i++) {//ищем самое лучшее действие
		if (strategy11[player][enemy][bomb11][bomb12][bomb21][bomb22][i] > max) {
			max = strategy11[player][enemy][bomb11][bomb12][bomb21][bomb22][i];
			action = i;
		}
		rewards[i] = strategy11[player][enemy][bomb11][bomb12][bomb21][bomb22][i];//предпологаемая награда за это действие
	}
	indexes[0] = player, indexes[1] = enemy, indexes[2] = bomb11;
	indexes[3] = bomb12, indexes[4] = bomb21, indexes[5] = bomb22;
	if (action == 0) bumb1 = 0, dir1 = 0;
	if (action == 1) bumb1 = 1;
	if (action == 2) dir1 = 1;
	if (action == 3) dir1 = 2;
	if (action == 4) dir1 = 3;
	if (action == 5) dir1 = 4;
	return action;
}
/*ищет по текущей ситуации лучшее движение для 2го игрока с вероятностью случайных дейчтвий,
  и записываем это состояние и вероятности в массивы возвращая номер действия*/
int Q_player2(int indexes[], float rewards[], float random) {
	if (((float)rand() / (float)RAND_MAX) < random) {
		int randomishe = rand() % 5 +1;
		if (randomishe == 0) bumb2 = 0, dir2 = 0;
		if (randomishe == 1) bumb2 = 1;
		if (randomishe == 2) dir2 = 1;
		if (randomishe == 3) dir2 = 2;
		if (randomishe == 4) dir2 = 3;
		if (randomishe == 5) dir2 = 4;
		return randomishe;
	}
	int index[12][2] = { {0,2},{0,0},//координаты ячеек где нет стен - пространство агента
						{1,3},{1,2},{1,1},{1,0},
						{2,2},{2,0},
						{3,3},{3,2},{3,1},{3,0} };
	int player = 0;//По-умолчанию на карте нет ничего
	int enemy = 0;
	int bomb11 = 0;
	int bomb12 = 0;
	int bomb21 = 0;
	int bomb22 = 0;

	for (int i = 0; i < 12; i++) {//Получаем координаты текущего стостояния мира
		if (players_and_walls[index[i][0]][index[i][1]] == 2) player = i + 1;
		if (players_and_walls[index[i][0]][index[i][1]] == 1) enemy = i + 1;
		if (booms[index[i][0]][index[i][1]] >= 1 && booms[index[i][0]][index[i][1]] <= 1.5 && bomb11 == 0) bomb11 = i + 1;
		if (booms[index[i][0]][index[i][1]] > 1.5 && booms[index[i][0]][index[i][1]] <= 2 && bomb12 == 0) bomb12 = i + 1;
		if (booms[index[i][0]][index[i][1]] >= 1 && booms[index[i][0]][index[i][1]] <= 1.5 && bomb11 != 0) bomb21 = i + 1;
		if (booms[index[i][0]][index[i][1]] > 1.5 && booms[index[i][0]][index[i][1]] <= 2 && bomb12 != 0) bomb22 = i + 1;
	}
	int action = 0;//По-умолчанию мы ничего не делаем
	float max = std::numeric_limits<float>::min();
	//Находим стратегию текущего стостояния мира
	for (int i = 1; i < 6; i++) {
		if (strategy12[player][enemy][bomb11][bomb12][bomb21][bomb22][i] > max) {
			max = strategy12[player][enemy][bomb11][bomb12][bomb21][bomb22][i];
			action = i;
		}
		rewards[i] = strategy12[player][enemy][bomb11][bomb12][bomb21][bomb22][i];
	}
	indexes[0] = player, indexes[1] = enemy, indexes[2] = bomb11, indexes[3] = bomb12, indexes[4] = bomb21, indexes[5] = bomb22;
	if (action == 0) bumb2 = 0, dir2 = 0;
	if (action == 1) bumb2 = 1;
	if (action == 2) dir2 = 1;
	if (action == 3) dir2 = 2;
	if (action == 4) dir2 = 3;
	if (action == 5) dir2 = 4;
	return action;
}

/*ищем по текущей ситуации лучшее движение для 1го игрока с вероятностью случайного движения,
  и записываем это состояние и вероятности в массивы возвращая номер действия*/
int Q_player21(int indexes[], float rewards[], float random) {
	//случайные действия
	if (((float)rand() / (float)RAND_MAX) < random) {
		int randomishe = rand() % 5 + 1;
		if (randomishe == 0) bumb1 = 0, dir1 = 0;
		if (randomishe == 1) bumb1 = 1;
		if (randomishe == 2) dir1 = 1;
		if (randomishe == 3) dir1 = 2;
		if (randomishe == 4) dir1 = 3;
		if (randomishe == 5) dir1 = 4;
		return randomishe;
	}

	//Не случайные действия
	int index[12][2] = { {1,3},{3,3},//координаты ячеек где нет стен - пространство агента
						{0,2},{1,2},{2,2},{3,2},
						{1,1},{3,1},
						{0,0},{1,0},{2,0},{3,0} };
	int player = 0;//По-умолчанию на карте нет ничего
	int enemy = 0;
	int bomb11 = 0;
	int bomb12 = 0;
	int bomb21 = 0;
	int bomb22 = 0;

	for (int i = 0; i < 12; i++) {
		if (players_and_walls[index[i][0]][index[i][1]] == 1) player = i + 1;//i+1 т.к. в стратегии нуль означает отсутствие
		if (players_and_walls[index[i][0]][index[i][1]] == 2) enemy = i + 1;
		if (booms[index[i][0]][index[i][1]] >= 1 && booms[index[i][0]][index[i][1]] <= 1.5 && bomb11 == 0) bomb11 = i + 1;
		if (booms[index[i][0]][index[i][1]] > 1.5 && booms[index[i][0]][index[i][1]] <= 2 && bomb12 == 0) bomb12 = i + 1;
		if (booms[index[i][0]][index[i][1]] >= 1 && booms[index[i][0]][index[i][1]] <= 1.5 && bomb11 != 0) bomb21 = i + 1;
		if (booms[index[i][0]][index[i][1]] > 1.5 && booms[index[i][0]][index[i][1]] <= 2 && bomb12 != 0) bomb22 = i + 1;
	}
	int action = 0;//По-умолчанию мы ничего не делаем
	float max = std::numeric_limits<float>::min();
	for (int i = 1; i < 6; i++) {//ищем самое лучшее действие
		if (strategy11[player][enemy][bomb11][bomb12][bomb21][bomb22][i] > max) {
			max = strategy11[player][enemy][bomb11][bomb12][bomb21][bomb22][i];
			action = i;
		}
		rewards[i] = strategy11[player][enemy][bomb11][bomb12][bomb21][bomb22][i];//предпологаемая награда за это действие
	}
	indexes[0] = player, indexes[1] = enemy, indexes[2] = bomb11;
	indexes[3] = bomb12, indexes[4] = bomb21, indexes[5] = bomb22;
	if (action == 0) bumb1 = 0, dir1 = 0;
	if (action == 1) bumb1 = 1;
	if (action == 2) dir1 = 1;
	if (action == 3) dir1 = 2;
	if (action == 4) dir1 = 3;
	if (action == 5) dir1 = 4;
	return action;
}
/*ищет по текущей ситуации лучшее движение для 2го игрока с вероятностью случайных дейчтвий,
  и записываем это состояние и вероятности в массивы возвращая номер действия*/
int Q_player22(int indexes[], float rewards[], float random) {
	if (((float)rand() / (float)RAND_MAX) < random) {
		int randomishe = rand() % 5 + 1;
		if (randomishe == 0) bumb2 = 0, dir2 = 0;
		if (randomishe == 1) bumb2 = 1;
		if (randomishe == 2) dir2 = 1;
		if (randomishe == 3) dir2 = 2;
		if (randomishe == 4) dir2 = 3;
		if (randomishe == 5) dir2 = 4;
		return randomishe;
	}
	int index[12][2] = { {0,2},{0,0},//координаты ячеек где нет стен - пространство агента
						{1,3},{1,2},{1,1},{1,0},
						{2,2},{2,0},
						{3,3},{3,2},{3,1},{3,0} };
	int player = 0;//По-умолчанию на карте нет ничего
	int enemy = 0;
	int bomb11 = 0;
	int bomb12 = 0;
	int bomb21 = 0;
	int bomb22 = 0;

	for (int i = 0; i < 12; i++) {//Получаем координаты текущего стостояния мира
		if (players_and_walls[index[i][0]][index[i][1]] == 2) player = i + 1;
		if (players_and_walls[index[i][0]][index[i][1]] == 1) enemy = i + 1;
		if (booms[index[i][0]][index[i][1]] >= 1 && booms[index[i][0]][index[i][1]] <= 1.5 && bomb11 == 0) bomb11 = i + 1;
		if (booms[index[i][0]][index[i][1]] > 1.5 && booms[index[i][0]][index[i][1]] <= 2 && bomb12 == 0) bomb12 = i + 1;
		if (booms[index[i][0]][index[i][1]] >= 1 && booms[index[i][0]][index[i][1]] <= 1.5 && bomb11 != 0) bomb21 = i + 1;
		if (booms[index[i][0]][index[i][1]] > 1.5 && booms[index[i][0]][index[i][1]] <= 2 && bomb12 != 0) bomb22 = i + 1;
	}
	int action = 0;//По-умолчанию мы ничего не делаем
	float max = std::numeric_limits<float>::min();
	//Находим стратегию текущего стостояния мира
	for (int i = 1; i < 6; i++) {
		if (strategy22[player][enemy][bomb11][bomb12][bomb21][bomb22][i] > max) {
			max = strategy22[player][enemy][bomb11][bomb12][bomb21][bomb22][i];
			action = i;
		}
		rewards[i] = strategy22[player][enemy][bomb11][bomb12][bomb21][bomb22][i];
	}
	indexes[0] = player, indexes[1] = enemy, indexes[2] = bomb11, indexes[3] = bomb12, indexes[4] = bomb21, indexes[5] = bomb22;
	if (action == 0) bumb2 = 0, dir2 = 0;
	if (action == 1) bumb2 = 1;
	if (action == 2) dir2 = 1;
	if (action == 3) dir2 = 2;
	if (action == 4) dir2 = 3;
	if (action == 5) dir2 = 4;
	return action;
}




//Корректируем вероятности зная действие которое произвел агент на данном шаге и 
void correcting1(int indexes[], float rewards[], float reward, int step, int step_future) {
	strategy11[indexes[0]][indexes[1]][indexes[2]][indexes[3]][indexes[4]][indexes[5]][step]
		+= strategy11[indexes[0]][indexes[1]][indexes[2]][indexes[3]][indexes[4]][indexes[5]][step] * reward * 0.01;//0.1 * (reward + 0.9 * rewards[step_future] - strategy11[indexes[0]][indexes[1]][indexes[2]][indexes[3]][indexes[4]][indexes[5]][step]);
}
void correcting2(int indexes[], float rewards[], float reward, int step, int step_future) {
	strategy12[indexes[0]][indexes[1]][indexes[2]][indexes[3]][indexes[4]][indexes[5]][step]
		+=strategy12[indexes[0]][indexes[1]][indexes[2]][indexes[3]][indexes[4]][indexes[5]][step] * reward * 0.01;// 0.1 * (reward + 0.9 * rewards[step_future] - strategy12[indexes[0]][indexes[1]][indexes[2]][indexes[3]][indexes[4]][indexes[5]][step]);//
}
void correcting21(int indexes[], float rewards[], float reward, int step, int step_future) {
	strategy21[indexes[0]][indexes[1]][indexes[2]][indexes[3]][indexes[4]][indexes[5]][step]
		+= strategy21[indexes[0]][indexes[1]][indexes[2]][indexes[3]][indexes[4]][indexes[5]][step] * reward * 0.01;//0.1 * (reward + 0.9 * rewards[step_future] - strategy21[indexes[0]][indexes[1]][indexes[2]][indexes[3]][indexes[4]][indexes[5]][step]);
}
void correcting22(int indexes[], float rewards[], float reward, int step, int step_future) {
	strategy22[indexes[0]][indexes[1]][indexes[2]][indexes[3]][indexes[4]][indexes[5]][step]
		+= strategy22[indexes[0]][indexes[1]][indexes[2]][indexes[3]][indexes[4]][indexes[5]][step] * reward * 0.01;// 0.1 * (reward + 0.9 * rewards[step_future] - strategy22[indexes[0]][indexes[1]][indexes[2]][indexes[3]][indexes[4]][indexes[5]][step]);//
}


//Функции контроля игрока с клавиатуры
void controller_1th() {
	if (_kbhit()) {
		switch (_getch()) // при нажатии клавиш, задаем направление (вверх, вниз, влево, вправо)
		{
		case 'd': dir1 = 1;  break;
		case 'a': dir1 = 2;  break;
		case 'w': dir1 = 3;  break;
		case 's': dir1 = 4;  break;
		case VK_SPACE: bumb1 = 1; break;
		}
	}
}
void controller_2th() {
	if (_kbhit()) {
		switch (_getch()) // при нажатии клавиш, задаем направление (вверх, вниз, влево, вправо)
		{
		case 'd': dir2 = 1;  break;
		case 'a': dir2 = 2;  break;
		case 'w': dir2 = 3;  break;
		case 's': dir2 = 4;  break;
		case VK_SPACE: bumb2 = 1;  break;
		}
	}
}

//обрабатывает изминения в мире за тик
void Game_tic() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			//FIRE!!!!!
			if (booms[i][j] == -1) booms[i][j] = 0;
		}
	}
	for (int i = N - 1; i >= 0; i--) {
		for (int j = M-1; j >=0 ; j--) {
			//Таймер бомбы
			if (booms[i][j] >= 1 && booms[i][j]<2) booms[i][j] += 0.2;
			//BOOOM!!!!
			if (booms[i][j] >= 2) {//BOOOM!!!!
				boombs--;//минус бомба
				booms[i][j] = -1;//огонь где была бомба
				players_and_walls[i][j] = 0;//Разрушение где была бомба
				int flags[4] = { 0,0,0,0 };//индикатор того что пламя уперлось в стенку
				for (int l = 1; l < 6; l++) {//Длина пламени = 6
					if ((i + l) < N && flags[0]==0) {//Пламя идет направо пока не упрется в стенку или конец карты
						if (booms[i + l][j] <= 0) {//По пути пламени нет бомбы
							if (players_and_walls[i + l][j] == 0) {//по пути пламени ничего нет
								booms[i + l][j] = -1;//Пламя
							}
							else {//На пути пламени что-то есть
								flags[0] = 1;
								if (players_and_walls[i + l][j] == 2 || players_and_walls[i + l][j] == 1) {
									players_and_walls[i + l][j] = 0;
									booms[i + l][j] = -1;//Пламя убивает игроков
								}
							}
						}
						else {//если по пути пламени есть бомба то пламя останавливается и взрывает ее
							booms[i + l][j] = 2;
							flags[0] = 1;
						}
					}
					if ((i - l) >= 0 && flags[1] == 0) {//Пламя идет налево пока не упрется в стенку или конец карты
						if (booms[i - l][j] <= 0) {//По пути пламени нет бомбы
							if (players_and_walls[i - l][j] == 0) {//по пути пламени ничего нет
								booms[i - l][j] = -1;//Пламя
							}
							else {//На пути пламени что-то есть
								flags[1] = 1;
								if (players_and_walls[i - l][j] == 2 || players_and_walls[i - l][j] == 1) {
									players_and_walls[i - l][j] = 0;
									booms[i - l][j] = -1;//Пламя убивает игроков, но стены не трогает
								}
							}
						}
						else {//если по пути пламени есть бомба то пламя останавливается и взрывает ее
							booms[i - l][j] = 2;
							flags[1] = 1;
						}
					}
					if ((j + l) < M && flags[2] == 0) {//Пламя идет направо пока не упрется в стенку или конец карты
						if (booms[i][j+l] <= 0) {//По пути пламени нет бомбы
							if (players_and_walls[i][j+l] == 0) {//по пути пламени ничего нет
								booms[i][j+l] = -1;//Пламя
							}
							else {//На пути пламени что-то есть
								flags[2] = 1;
								if (players_and_walls[i][j+l] == 2 || players_and_walls[i][j+l] == 1) {
									players_and_walls[i][j+l] = 0;
									booms[i][j+l] = -1;//Пламя убивает игроков
								}
							}
						}
						else {//если по пути пламени есть бомба то пламя останавливается и взрывает ее
							booms[i][j+l] = 2;
							flags[2] = 1;
						}
					}
					if ((j - l) >= 0 && flags[3] == 0) {//Пламя идет направо пока не упрется в стенку или конец карты
						if (booms[i][j - l] <= 0) {//По пути пламени нет бомбы
							if (players_and_walls[i][j - l] == 0) {//по пути пламени ничего нет
								booms[i][j - l] = -1;//Пламя
							}
							else {//На пути пламени что-то есть
								flags[3] = 1;
								if (players_and_walls[i][j - l] == 2 || players_and_walls[i][j - l] == 1) {
									players_and_walls[i][j - l] = 0;
									booms[i][j - l] = -1;//Пламя убивает игроков
								}
							}
						}
						else {//если по пути пламени есть бомба то пламя останавливается и взрывает ее
							booms[i][j - l] = 2;
							flags[3] = 1;
						}
					}
				}
			}

			//управление 1го игрока
			if (players_and_walls[i][j] == 1) {
				if (bumb1 == 1 && boombs < MAX_bombs) {
					booms[i][j] = 1;//сброс бомбы
					boombs++;
					bumb1 = 0;
				}
				switch (dir1)
				{
				//направо
				case 1: {
					if (players_and_walls[i + 1][j] == 0 && i+1<N) {
						players_and_walls[i + 1][j] = 1;
						players_and_walls[i][j] = 0;
					}
					dir1 = 0;
					break;
				}
				//налево
				case 2: {
					if (players_and_walls[i - 1][j] == 0 && i-1>=0) {
						players_and_walls[i - 1][j] = 1;
						players_and_walls[i][j] = 0;
					}
					dir1 = 0;
					break;
				}
				//Вверх
				case 3: {
					if (players_and_walls[i][j+1] == 0 && j+1<M) {
						players_and_walls[i][j+1] = 1;
						players_and_walls[i][j] = 0;
					}
					dir1 = 0;
					break;
				}
				//Вниз
				case 4: {
					if (players_and_walls[i][j-1] == 0 && j-1>=0) {
						players_and_walls[i][j-1] = 1;
						players_and_walls[i][j] = 0;
					}
					dir1 = 0;
					break;
				}
				}
			}
			//управление 2го игрока
			if (players_and_walls[i][j] == 2) {
				if (bumb2 == 1 && boombs < MAX_bombs) {
					booms[i][j] = 1;//сброс бомбы
					boombs++;
					bumb2 = 0;
				}
				switch (dir2)
				{
					//направо
				case 1: {
					if (players_and_walls[i + 1][j] == 0 && i + 1 < N) {
						players_and_walls[i + 1][j] = 2;
						players_and_walls[i][j] = 0;
					}
					dir2 = 0;
					break;
				}
					  //налево
				case 2: {
					if (players_and_walls[i - 1][j] == 0 && i - 1 >= 0) {
						players_and_walls[i - 1][j] = 2;
						players_and_walls[i][j] = 0;
					}
					dir2 = 0;
					break;
				}
				//Вверх
				case 3: {
					if (players_and_walls[i][j + 1] == 0 && j + 1 < M) {
						players_and_walls[i][j + 1] = 2;
						players_and_walls[i][j] = 0;
					}
					dir2 = 0;
					break;
				}
					  //Вниз
				case 4: {
					if (players_and_walls[i][j - 1] == 0 && j - 1 >= 0) {
						players_and_walls[i][j - 1] = 2;
						players_and_walls[i][j] = 0;
					}
					dir2 = 0;
					break;
				}
				}
			}
		}
	}
}

//функция общий отрисовки
void Display()
{
	glClear(GL_COLOR_BUFFER_BIT); // очищаем окно перед началом отрисовки
	glClearColor(0.8, 0.8, 0.8, 1.0);

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			//Отрисовываем стенки
			if (players_and_walls[i][j] == -1) {
				glColor3ub(239, 146, 95);
				glRectf(i * scale, j * scale, (i + 1) * scale, (j + 1) * scale);
			}
			//Рисуем бомбы
			if (booms[i][j] >= 1) {
				glColor3f((booms[i][j]) - 1, 0, 0);
				float koeff = (2 - booms[i][j]) / 2;
				glRectf(i * scale + scale * koeff, j * scale + scale * koeff, (i + 1) * scale - scale * koeff, (j + 1) * scale - scale * koeff);
			}
			//Рисуем огонь
			if (booms[i][j] <= -1) {
				glColor3ub(255, 255, 255);
				glRectf(i * scale, j * scale, (i + 1) * scale, (j + 1) * scale);
			}
			//Рисуем 1го игрока
			else if (players_and_walls[i][j] == 1) {
				glColor3ub(0, 178, 88);
				glBegin(GL_POLYGON);
				glVertex3f(i * scale, j * scale, 0.0f);
				glVertex3f((i + 1) * scale, j * scale, 0.0f);
				glVertex3f((i + 1) * scale - scale / 4, (j + 1) * scale, 0.0f);
				glVertex3f(i * scale + scale / 4, (j + 1) * scale, 0.0f);
				glEnd();
			}
			//Рисуем 2го игрока
			else if (players_and_walls[i][j] == 2) {
				glColor3ub(0, 0, 128);
				glBegin(GL_POLYGON);
				glVertex3f(i * scale, j * scale, 0.0f);
				glVertex3f((i + 1) * scale, j * scale, 0.0f);
				glVertex3f((i + 1) * scale - scale / 4, (j + 1) * scale, 0.0f);
				glVertex3f(i * scale + scale / 4, (j + 1) * scale, 0.0f);
				glEnd();
			}
		}
	}
	glFlush(); // выводим на экран всё вышеописанное
	glutSwapBuffers();
}

int iterations = 1;
int Game_learn() {
	float random = 0.1;//как часто делаем случайные действия

	int tiks = 0;
	int live1 = 1, live2 = 1;

	//запоминаем последние 5 шагов
	float steps_revard1[6][6] = { 0 };
	int steps_index1[6][6] = { 0 };
	float steps_revard2[6][6] = { 0 };
	int steps_index2[6][6] = { 0 };
	int step1[6], step2[6];
	Start_generation();
	//сессия, ограничиваем ее 1000 тиками
	while (tiks < 1000 && live1 + live2 == 2) {
		tiks++;

		//Делаем ход и запоминаем последние несколько
		step1[0] = Q_player1(steps_index1[0], steps_revard1[0], random);
		step2[0] = Q_player2(steps_index2[0], steps_revard2[0], random);
		Game_tic();
		for (int i = 5; i > 0; i--) {
			for (int j = 0; j < 6; j++) {
				steps_index1[i][j] = steps_index1[i - 1][j];
				steps_revard1[i][j] = steps_revard1[i - 1][j];
				steps_index2[i][j] = steps_index2[i - 1][j];
				steps_revard2[i][j] = steps_revard2[i - 1][j];
			}
			step1[i] = step1[i - 1];
			step2[i] = step2[i - 1];
		}

		//Поиск выживших
		live1 = 0, live2 = 0;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				if (players_and_walls[i][j] == 1) live1 = 1;
				if (players_and_walls[i][j] == 2) live2 = 1;
			}
		}
	}


	//Изменяем вероятности исходя из последних шагов
	//Распределяем пряники и кнуты, Выводим результат текущей итерации
	if (live1 == 1 && live2 == 0) {//1й win
		for (int i = 3; i > 0; i--) {
			correcting1(steps_index1[i], steps_revard1[i - 1], 1, step1[i], step1[i-1]);
			correcting2(steps_index2[i], steps_revard2[i - 1], -1, step2[i], step1[i - 1]);
		}
		//printf("%i %04i %i ", iterations, tiks, 1);
		wins1++;
	}
	else {
		if (live1 == 0 && live2 == 1) {//2й win
			for (int i = 5; i > 1; i--) {
				correcting1(steps_index1[i], steps_revard1[i - 1], -1, step1[i], step1[i - 1]);
				correcting2(steps_index2[i], steps_revard2[i - 1], 1, step2[i], step1[i - 1]);
			}
			//printf("%i %04i %i ", iterations, tiks, 2);
			wins2++;
		}
		else {//никто не win
			//printf("%i %04i %i ", iterations, tiks, 0);
			for (int i = 5; i > 1; i--) {
				correcting1(steps_index1[i], steps_revard1[i - 1], -1, step1[i], step1[i - 1]);
				correcting2(steps_index2[i], steps_revard2[i - 1], -1, step2[i], step1[i - 1]);
			}
		}
	}
	if (iterations % 10000 == 0) winsy1 = wins1, wins1 = 0, winsy2 = wins2, wins2 = 0;
	//Печатаем винрейт 1го и 2го агента
	//printf("%f %f\n", (float)winsy1/10000.0, (float)winsy2 / 10000.0);
	iterations++;
	return tiks;
}

int iterations2 = 1;
int Game_learn2() {
	float random = 0.1;//как часто делаем случайные действия

	int tiks = 0;
	int live1 = 1, live2 = 1;

	//запоминаем последние 5 шагов
	float steps_revard1[6][6] = { 0 };
	int steps_index1[6][6] = { 0 };
	float steps_revard2[6][6] = { 0 };
	int steps_index2[6][6] = { 0 };
	int step1[6], step2[6];
	Start_generation();
	//сессия, ограничиваем ее 1000 тиками
	while (tiks < 1000 && live1 + live2 == 2) {
		tiks++;

		//Делаем ход и запоминаем последние несколько
		step1[0] = Q_player21(steps_index1[0], steps_revard1[0], random);
		step2[0] = Q_player22(steps_index2[0], steps_revard2[0], random);
		Game_tic();
		for (int i = 5; i > 0; i--) {
			for (int j = 0; j < 6; j++) {
				steps_index1[i][j] = steps_index1[i - 1][j];
				steps_revard1[i][j] = steps_revard1[i - 1][j];
				steps_index2[i][j] = steps_index2[i - 1][j];
				steps_revard2[i][j] = steps_revard2[i - 1][j];
			}
			step1[i] = step1[i - 1];
			step2[i] = step2[i - 1];
		}

		//Поиск выживших
		live1 = 0, live2 = 0;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				if (players_and_walls[i][j] == 1) live1 = 1;
				if (players_and_walls[i][j] == 2) live2 = 1;
			}
		}
	}


	//Изменяем вероятности исходя из последних шагов
	//Распределяем пряники и кнуты, Выводим результат текущей итерации
	if (live1 == 1 && live2 == 0) {//1й win
		for (int i = 3; i > 0; i--) {
			correcting21(steps_index1[i], steps_revard1[i - 1], 1, step1[i], step1[i - 1]);
			correcting22(steps_index2[i], steps_revard2[i - 1], -1, step2[i], step1[i - 1]);
		}
		//printf("%i %04i %i ", iterations2, tiks, 1);
		wins1++;
	}
	else {
		if (live1 == 0 && live2 == 1) {//2й win
			for (int i = 5; i > 1; i--) {
				correcting21(steps_index1[i], steps_revard1[i - 1], -1, step1[i], step1[i - 1]);
				correcting22(steps_index2[i], steps_revard2[i - 1], 1, step2[i], step1[i - 1]);
			}
			//printf("%i %04i %i ", iterations2, tiks, 2);
			wins2++;
		}
		else {//никто не win
			//printf("%i %04i %i ", iterations2, tiks, 0);
			for (int i = 5; i > 1; i--) {
				correcting21(steps_index1[i], steps_revard1[i - 1], -1, step1[i], step1[i - 1]);
				correcting22(steps_index2[i], steps_revard2[i - 1], -1, step2[i], step1[i - 1]);
			}
		}
	}
	if (iterations2 % 10000 == 0) winsy1 = wins1, wins1 = 0, winsy2 = wins2, wins2 = 0;
	//Печатаем винрейт 1го и 2го агента
	//printf("%f %f\n", (float)winsy1 / 10000.0, (float)winsy2 / 10000.0);
	iterations2++;
	return tiks;
}

void timer(int = 0) // Таймер игры(промежуток времени, в котором будет производится все процессы)
{
	float steps_revard[6];
	int steps_index[6];
	//Q_player1(steps_index, steps_revard, 0.1);
	//Q_player2(steps_index, steps_revard, 0.1);
	controller_2th();
	Game_tic();
	Display(); // отрисовываем 

	//Перезапуск если кто-то сдох
	int live1 = 0, live2 = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			if (players_and_walls[i][j] == 1) live1 = 1;
			if (players_and_walls[i][j] == 2) live2 = 1;
		}
	}
	if (live1 + live2 <= 1) Start_generation();


	glutTimerFunc(3000, timer, 0); // новый вызов таймера( 300 - промежуток времени(в милисекундах), через который он будет вызыватся)
}

void save_strategies() {
	wins1 = 0, wins2 = 0;//кол-во выигрышей 1го и 2го игроков вообще
	winsy1 = 0, winsy2 = 0;//к-во побед 1го и 2го агента за 1000 последних игр
	iterations = 1;
	//1000 итераций
	Start_generation();
	random_strategy();
	for (int i = 0; i < 1000; i++)	Game_learn();
	char name1000_1[] = "1000_1.txt";
	char name1000_2[] = "1000_2.txt";
	save_strategy1(name1000_1);
	save_strategy2(name1000_2);

	//10 000 итераций
	wins1 = 0, wins2 = 0;//кол-во выигрышей 1го и 2го игроков вообще
	winsy1 = 0, winsy2 = 0;//к-во побед 1го и 2го агента за 1000 последних игр
	iterations = 1;
	Start_generation();
	random_strategy();
	for (int i = 0; i < 10000; i++)	Game_learn();
	char name10000_1[] = "10000_1.txt";
	char name10000_2[] = "10000_2.txt";
	save_strategy1(name10000_1);
	save_strategy2(name10000_2);

	//100 000 итераций
	wins1 = 0, wins2 = 0;//кол-во выигрышей 1го и 2го игроков вообще
	winsy1 = 0, winsy2 = 0;//к-во побед 1го и 2го агента за 1000 последних игр
	iterations = 1;
	Start_generation();
	random_strategy();
	for (int i = 0; i < 100000; i++)	Game_learn();
	char name100000_1[] = "100000_1.txt";
	char name100000_2[] = "100000_2.txt";
	save_strategy1(name100000_1);
	save_strategy2(name100000_2);


	//1 000 000 итераций
	wins1 = 0, wins2 = 0;//кол-во выигрышей 1го и 2го игроков вообще
	winsy1 = 0, winsy2 = 0;//к-во побед 1го и 2го агента за 1000 последних игр
	iterations = 1;
	Start_generation();
	random_strategy();
	for (int i = 0; i < 1000000; i++)	Game_learn();
	char name1000000_1[] = "1000000_1.txt";
	char name1000000_2[] = "1000000_2.txt";
	save_strategy1(name1000000_1);
	save_strategy2(name1000000_2);

	
	//10 000 000 итераций
	wins1 = 0, wins2 = 0;//кол-во выигрышей 1го и 2го игроков вообще
	winsy1 = 0, winsy2 = 0;//к-во побед 1го и 2го агента за 1000 последних игр
	iterations = 1;
	Start_generation();
	random_strategy();
	for (int i = 0; i < 10000000; i++)	Game_learn();
	char name10000000_1[] = "10000000_1.txt";
	char name10000000_2[] = "10000000_2.txt";
	save_strategy1(name10000000_1);
	save_strategy2(name10000000_2);
}

//Возвращает результат одной итерации
int test_iteration1VS2() {
	float random = 0.1;//как часто делаем случайные действия
	int tiks = 0;
	int live1 = 1, live2 = 1;

	//запоминаем последние 5 шагов
	float steps_revard1[6][6] = { 0 };
	int steps_index1[6][6] = { 0 };
	int step1[6];
	Start_generation();
	//сессия, ограничиваем ее 1000 тиками
	while (tiks < 1000 && live1 + live2 == 2) {
		tiks++;

		//Делаем ход и запоминаем последние несколько
		Q_player1(steps_index1[0], steps_revard1[0], random);
		Q_player22(steps_index1[0], steps_revard1[0], random);
		Game_tic();

		//Поиск выживших
		live1 = 0, live2 = 0;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				if (players_and_walls[i][j] == 1) live1 = 1;
				if (players_and_walls[i][j] == 2) live2 = 1;
			}
		}
	}
	if (live1 == 1 && live2 == 0) {//1й win
		return 1;
	}
	else {
		if (live1 == 0 && live2 == 1) {//2й win
			return 2;
		}
		else {//никто не win
			return 0;
		}
	}

}
int test_iteration2VS1() {
	float random = 0.1;//как часто делаем случайные действия
	int tiks = 0;
	int live1 = 1, live2 = 1;

	//запоминаем последние 5 шагов
	float steps_revard1[6][6] = { 0 };
	int steps_index1[6][6] = { 0 };
	int step1[6];
	Start_generation();
	//сессия, ограничиваем ее 1000 тиками
	while (tiks < 1000 && live1 + live2 == 2) {
		tiks++;

		//Делаем ход и запоминаем последние несколько
		Q_player21(steps_index1[0], steps_revard1[0], random);
		Q_player2(steps_index1[0], steps_revard1[0], random);
		Game_tic();

		//Поиск выживших
		live1 = 0, live2 = 0;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				if (players_and_walls[i][j] == 1) live1 = 1;
				if (players_and_walls[i][j] == 2) live2 = 1;
			}
		}
	}
	if (live1 == 1 && live2 == 0) {//1й win
		return 1;
	}
	else {
		if (live1 == 0 && live2 == 1) {//2й win
			return 2;
		}
		else {//никто не win
			return 0;
		}
	}

}

//сравнивает 2 стратегии записанные в файлы и выдает в консоль результат
void test_strategy(int one, int two, int testN, float result[]){
	Start_generation();
	//null_strategy();
	random_strategy();
	for (int i = 0; i < one; i++) {
		Game_learn();
	}
	for (int i = 0; i < two; i++) {
		Game_learn2();
	}
	
	int win1 = 0;
	int win2 = 0;
	for (int i = 0; i < testN; i++) {
		int win = test_iteration1VS2();
		if (win == 1) win1++;
		if (win == 2) win2++;
	}
	result[0] = (float)win1 / (float)testN;
	result[1] = (float)win2 / (float)testN;
	//printf("%i -  %f  VS  %i  -  %f\n",one, (float)win1 / (float)testN, two, (float)win2 / (float)testN);

	win1 = 0;
	win2 = 0;
	for (int i = 0; i < testN; i++) {
		int win = test_iteration2VS1();
		if (win == 1) win1++;
		if (win == 2) win2++;
	}
	result[2] = (float)win1 / (float)testN;
	result[3] = (float)win2 / (float)testN;
	//printf("%i -  %f  VS  %i  -  %f\n", two, (float)win1 / (float)testN, one, (float)win2 / (float)testN);
	//printf("\n\n");

}
FILE* sravni;
void srani(int iter) {
	fopen_s(&sravni, "sravni.txt", "w");
	fprintf(sravni, "3vs3_1 3vs3_2 3vs4_1 3vs4_2 3vs5_1 3vs5_2 3vs6_1 3vs6_2 4vs3_1 4vs3_2 4vs4_1 4vs4_2 4vs5_1 4vs5_2 4vs6_1 4vs6_2 5vs3_1 5vs3_2 5vs4_1 5vs4_2 5vs5_1 5vs5_2 5vs6_1 5vs6_2 6vs3_1 6vs3_2 6vs4_1 6vs4_2 6vs5_1 6vs5_2 6vs6_1 6vs6_2\n");
	float result[4];

	for (int i = 1; i <= iter; i++) {
		test_strategy(1000, 1000, 100000, result);
		fprintf(sravni, "%f %f ", result[0], result[1]);

		test_strategy(1000, 10000, 100000, result);
		fprintf(sravni, "%f %f ", result[0], result[1]);

		test_strategy(1000, 100000, 100000, result);
		fprintf(sravni, "%f %f ", result[0], result[1]);

		test_strategy(1000, 1000000, 100000, result);
		fprintf(sravni, "%f %f ", result[0], result[1]);




		test_strategy(10000, 1000, 100000, result);
		fprintf(sravni, "%f %f ", result[0], result[1]);

		test_strategy(10000, 10000, 100000, result);
		fprintf(sravni, "%f %f ", result[0], result[1]);

		test_strategy(10000, 100000, 100000, result);
		fprintf(sravni, "%f %f ", result[0], result[1]);

		test_strategy(10000, 1000000, 100000, result);
		fprintf(sravni, "%f %f ", result[0], result[1]);



		test_strategy(100000, 1000, 100000, result);
		fprintf(sravni, "%f %f ", result[0], result[1]);

		test_strategy(100000, 10000, 100000, result);
		fprintf(sravni, "%f %f ", result[0], result[1]);

		test_strategy(100000, 100000, 100000, result);
		fprintf(sravni, "%f %f ", result[0], result[1]);

		test_strategy(100000, 1000000, 100000, result);
		fprintf(sravni, "%f %f ", result[0], result[1]);


		test_strategy(1000000, 1000, 100000, result);
		fprintf(sravni, "%f %f ", result[0], result[1]);

		test_strategy(1000000, 10000, 100000, result);
		fprintf(sravni, "%f %f ", result[0], result[1]);

		test_strategy(1000000, 100000, 100000, result);
		fprintf(sravni, "%f %f ", result[0], result[1]);

		test_strategy(1000000, 1000000, 100000, result);
		fprintf(sravni, "%f %f\n", result[0], result[1]);



		printf("%i\n", i);
	}


	fclose(sravni);
}
	

FILE* alalys;
void analis() {
	Start_generation();
	random_strategy();
	fopen_s(&alalys, "analitik_wins.txt", "w");
	float win1 = 0, win2 = 0, wins1 = 0, wins2 = 0;
	fprintf(alalys, "1th 2th tiks\n");
	for (int k = 1; k < 10000000; k++) {
		int tiks = Game_learn();
		int live1 = 0, live2 = 0;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				if (players_and_walls[i][j] == 1) live1 = 1;
				if (players_and_walls[i][j] == 2) live2 = 1;
			}
		}
		if (live1 == 1 && live2 == 0) win1++;
		if (live1 == 0 && live2 == 1) win2++;
		if (k % 1000 == 0) {
			float a = win1 / 1000.0;
			float b = win2 / 1000.0;
			fprintf(alalys,"%f %f %i\n",a,b,tiks);
			win1 = 0, win2 = 0;
		}
	}
	fclose(alalys);
}

int main(int argc, char** argv)
{
	srand(time(0));
	//analis();
	//srani(100);
	Start_generation();
	//null_strategy();
	random_strategy();
	for (int i = 0; i < 1000000; i++)	Game_learn();

	// следующие функции абсолютно идиентичных почти во всех программах на OpenGL
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(w, h); //создаем окно (w - ширина, h - высота)
	glutCreateWindow("BoomberRL"); //название окна
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glutDisplayFunc(Display); //вызывает начальную функцию, в нашем случае это главная функция отрисовки - Display
	glutTimerFunc(300, timer, 0); //задаем рекурсивный таймер.
	glutMainLoop();

	return 0;
}
