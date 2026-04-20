/*
 * 全屏动画演示 - 240x240 ST7789 TFT LCD
 * 包含多种动画效果循环播放:
 *   1. 弹跳小球
 *   2. 扩散光环
 *   3. 彩色扫屏
 *   4. 星空粒子
 *   5. 旋转方块
 */

#include "main.h"
#include <stdlib.h>
#include <string.h>
#include "math.h"

/* 屏幕尺寸 */
#define SCREEN_W  240
#define SCREEN_H  240
#define SCREEN_CX 120
#define SCREEN_CY 120

/* 动画帧延时 (ms) */
#define FRAME_DELAY  20

/* RGB565 颜色合成宏 */
#define RGB565(r,g,b) (((uint16_t)(r & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | ((uint16_t)(b >> 3)))

/* 彩虹色表 (12色) */
static const uint16_t rainbow[12] = {
	RED, BRED, YELLOW, GRED, GREEN, GBLUE,
	CYAN, BLUE, LBBLUE, MAGENTA, LIGHTGREEN, WHITE
};

/* ============ 工具函数 ============ */

/* 简易 sin 近似 (输入 0~359 角度, 返回 -1000~1000) */
static int16_t isin(int16_t deg)
{
	/* 将角度映射到 0~359 */
	while(deg < 0)   deg += 360;
	while(deg >= 360) deg -= 360;
	/* Bhaskara I 近似 sin: 4x(180-x) / (40500 - x(180-x)), 精度 ~0.001 */
	if(deg <= 180) {
		int32_t x = deg;
		return (int16_t)(4000 * x * (180 - x) / (40500 - x * (180 - x)));
	} else {
		int32_t x = deg - 180;
		return (int16_t)(-4000 * x * (180 - x) / (40500 - x * (180 - x)));
	}
}

/* cos 近似 */
static int16_t icos(int16_t deg)
{
	return isin(deg + 90);
}

/* ============ 动画1: 弹跳小球 ============ */
#define BALL_COUNT 6

typedef struct {
	int16_t x, y;
	int16_t dx, dy;
	int16_t r;
	uint16_t color;
} Ball;

static void anim_bouncing_balls(uint16_t duration_frames)
{
	Ball balls[BALL_COUNT];
	uint16_t i, f;

	/* 初始化小球 */
	for(i = 0; i < BALL_COUNT; i++) {
		balls[i].r     = 8 + (i % 3) * 4;
		balls[i].x     = 20 + (i * 37) % 200;
		balls[i].y     = 20 + (i * 53) % 200;
		balls[i].dx    = 2 + (i % 3);
		balls[i].dy    = 2 + ((i + 1) % 3);
		balls[i].color = rainbow[i * 2 % 12];
	}

	LCD_Clear(BLACK);

	for(f = 0; f < duration_frames; f++) {
		for(i = 0; i < BALL_COUNT; i++) {
			int16_t r = balls[i].r;

			/* 擦除旧位置 (用黑色填充方块) */
			LCD_Display_Color(balls[i].x - r, balls[i].y - r,
			                  balls[i].x + r, balls[i].y + r, BLACK);

			/* 更新位置 */
			balls[i].x += balls[i].dx;
			balls[i].y += balls[i].dy;

			/* 边界反弹 */
			if(balls[i].x <= r)      { balls[i].x = r;              balls[i].dx = -balls[i].dx; }
			if(balls[i].x >= SCREEN_W - r - 1) { balls[i].x = SCREEN_W - r - 1; balls[i].dx = -balls[i].dx; }
			if(balls[i].y <= r)      { balls[i].y = r;              balls[i].dy = -balls[i].dy; }
			if(balls[i].y >= SCREEN_H - r - 1) { balls[i].y = SCREEN_H - r - 1; balls[i].dy = -balls[i].dy; }

			/* 绘制新位置 (填充圆用方块近似) */
			LCD_Display_Color(balls[i].x - r, balls[i].y - r,
			                  balls[i].x + r, balls[i].y + r, balls[i].color);
		}

		/* 碰撞检测 - 简单弹性碰撞 */
		{
			int16_t j, k;
			for(j = 0; j < BALL_COUNT - 1; j++) {
				for(k = j + 1; k < BALL_COUNT; k++) {
					int16_t dx = balls[j].x - balls[k].x;
					int16_t dy = balls[j].y - balls[k].y;
					int16_t min_dist = balls[j].r + balls[k].r;
					if(dx * dx + dy * dy < min_dist * min_dist) {
						/* 交换速度 */
						int16_t tmp;
						tmp = balls[j].dx; balls[j].dx = balls[k].dx; balls[k].dx = tmp;
						tmp = balls[j].dy; balls[j].dy = balls[k].dy; balls[k].dy = tmp;
					}
				}
			}
		}

		delay_ms(FRAME_DELAY);
	}
}

/* ============ 动画2: 扩散光环 (雷达效果) ============ */
/* 使用三角扇形填充代替逐像素绘制, 只用 LCD_Display_Color */
static void anim_radar_sweep(uint16_t duration_frames)
{
	uint16_t f;
	uint16_t angle = 0;

	LCD_Clear(BLACK);

	/* 先画好静态背景: 同心圆环 + 中心十字 */
	{
		int16_t r, a;
		for(r = 30; r <= 110; r += 40) {
			for(a = 0; a < 360; a += 4) {
				int16_t px = SCREEN_CX + (icos(a) * r) / 1000;
				int16_t py = SCREEN_CY + (isin(a) * r) / 1000;
				if(px >= 0 && px < SCREEN_W && py >= 0 && py < SCREEN_H)
					LCD_Display_DrawPoint(px, py, RGB565(0, 40, 0));
			}
		}
		LCD_Display_Color(SCREEN_CX - 3, SCREEN_CY, SCREEN_CX + 3, SCREEN_CY, GREEN);
		LCD_Display_Color(SCREEN_CX, SCREEN_CY - 3, SCREEN_CX, SCREEN_CY + 3, GREEN);
	}

	for(f = 0; f < duration_frames; f++) {
		/* 用三角扇形从中心向外填充当前扫线方向的扇区 */
		/* 每帧只画 1° 宽的三角形, 用水平线近似 */
		int16_t dx, dy, s;
		int16_t prev_angle = angle;

		/* 擦除上一帧的扫线: 画 3 条暗绿线做渐隐 */
		{
			int16_t fade1 = (prev_angle + 357) % 360;
			int16_t fade2 = (prev_angle + 358) % 360;
			int16_t fade3 = (prev_angle + 359) % 360;
			int16_t fdx, fdy, fs;

			fdx = icos(fade1); fdy = isin(fade1);
			for(fs = 4; fs <= 110; fs += 4) {
				int16_t px = SCREEN_CX + (fdx * fs) / 1000;
				int16_t py = SCREEN_CY + (fdy * fs) / 1000;
				if(px >= 0 && px < SCREEN_W && py >= 0 && py < SCREEN_H)
					LCD_Display_DrawPoint(px, py, BLACK);
			}
			fdx = icos(fade2); fdy = isin(fade2);
			for(fs = 4; fs <= 110; fs += 4) {
				int16_t px = SCREEN_CX + (fdx * fs) / 1000;
				int16_t py = SCREEN_CY + (fdy * fs) / 1000;
				if(px >= 0 && px < SCREEN_W && py >= 0 && py < SCREEN_H)
					LCD_Display_DrawPoint(px, py, RGB565(0, 8, 0));
			}
			fdx = icos(fade3); fdy = isin(fade3);
			for(fs = 4; fs <= 110; fs += 4) {
				int16_t px = SCREEN_CX + (fdx * fs) / 1000;
				int16_t py = SCREEN_CY + (fdy * fs) / 1000;
				if(px >= 0 && px < SCREEN_W && py >= 0 && py < SCREEN_H)
					LCD_Display_DrawPoint(px, py, RGB565(0, 20, 0));
			}
		}

		/* 画当前扫线 (亮绿) */
		dx = icos(angle);
		dy = isin(angle);
		for(s = 4; s <= 110; s += 3) {
			int16_t px = SCREEN_CX + (dx * s) / 1000;
			int16_t py = SCREEN_CY + (dy * s) / 1000;
			if(px >= 0 && px < SCREEN_W && py >= 0 && py < SCREEN_H)
				LCD_Display_DrawPoint(px, py, GREEN);
		}

		/* 随机闪烁的"目标点" */
		if(f % 20 == 0) {
			int16_t tr = 15 + (f * 7) % 80;
			int16_t ta = (f * 13) % 360;
			int16_t tx = SCREEN_CX + (icos(ta) * tr) / 1000;
			int16_t ty = SCREEN_CY + (isin(ta) * tr) / 1000;
			if(tx >= 2 && tx < SCREEN_W - 2 && ty >= 2 && ty < SCREEN_H - 2)
				LCD_Display_Color(tx - 2, ty - 2, tx + 2, ty + 2, RED);
		}

		/* 重画被覆盖的圆环上的点 */
		if(f % 60 == 0) {
			int16_t r, a;
			for(r = 30; r <= 110; r += 40) {
				for(a = 0; a < 360; a += 4) {
					int16_t px = SCREEN_CX + (icos(a) * r) / 1000;
					int16_t py = SCREEN_CY + (isin(a) * r) / 1000;
					if(px >= 0 && px < SCREEN_W && py >= 0 && py < SCREEN_H)
						LCD_Display_DrawPoint(px, py, RGB565(0, 40, 0));
				}
			}
		}

		angle = (angle + 2) % 360;
		delay_ms(15);
	}
}

/* ============ 动画3: 彩色水平扫屏 ============ */
static void anim_color_sweep(uint16_t duration_frames)
{
	uint16_t f, y;
	uint16_t hue_offset = 0;

	for(f = 0; f < duration_frames; f++) {
		/* 每帧画 12 行以填满屏幕 */
		for(y = 0; y < SCREEN_H; y += 2) {
			uint16_t r, g, b;
			uint16_t phase = (y + hue_offset) % 360;

			/* HSV -> RGB565 (S=1, V=1, H=phase) */
			if(phase < 60) {
				r = 255; g = phase * 255 / 60; b = 0;
			} else if(phase < 120) {
				r = (120 - phase) * 255 / 60; g = 255; b = 0;
			} else if(phase < 180) {
				r = 0; g = 255; b = (phase - 120) * 255 / 60;
			} else if(phase < 240) {
				r = 0; g = (240 - phase) * 255 / 60; b = 255;
			} else if(phase < 300) {
				r = (phase - 240) * 255 / 60; g = 0; b = 255;
			} else {
				r = 255; g = 0; b = (360 - phase) * 255 / 60;
			}

			LCD_Display_Color(0, y, SCREEN_W - 1, y + 1, RGB565(r, g, b));
		}

		hue_offset = (hue_offset + 3) % 360;
		delay_ms(FRAME_DELAY);
	}
}

/* ============ 动画4: 星空粒子 (逐列整体刷新) ============ */
#define STAR_COUNT 50

typedef struct {
	int16_t x, y;
	int16_t speed;
	uint16_t brightness;
} Star;

static void anim_starfield(uint16_t duration_frames)
{
	Star stars[STAR_COUNT];
	uint16_t f, i;

	/* 初始化星星 */
	for(i = 0; i < STAR_COUNT; i++) {
		stars[i].x = rand() % SCREEN_W;
		stars[i].y = rand() % SCREEN_H;
		stars[i].speed = 1 + (rand() % 4);
		stars[i].brightness = rand() % 3;
	}

	LCD_Clear(BLACK);

	for(f = 0; f < duration_frames; f++) {
		/* 分批刷新: 每帧刷新 24 列, 10 帧刷完全部 240 列 */
		uint16_t col_start = (f % 10) * 24;
		uint16_t col_end   = col_start + 24;
		uint16_t col;

		for(col = col_start; col < col_end; col++) {
			/* 整列先刷黑 */
			LCD_Display_Color(col, 0, col, SCREEN_H - 1, BLACK);
		}

		/* 更新所有星星位置 */
		for(i = 0; i < STAR_COUNT; i++) {
			stars[i].x -= stars[i].speed;
			if(stars[i].x < 0) {
				stars[i].x = SCREEN_W - 1;
				stars[i].y = rand() % SCREEN_H;
				stars[i].speed = 1 + (rand() % 4);
				stars[i].brightness = rand() % 3;
			}
		}

		/* 画星星 (只画在当前刷新列范围内的) */
		for(i = 0; i < STAR_COUNT; i++) {
			if(stars[i].x < col_start || stars[i].x >= col_end) continue;
			{
				uint16_t c;
				int16_t sx = stars[i].x;
				int16_t sy = stars[i].y;
				switch(stars[i].brightness) {
					case 0:  c = RGB565(60, 60, 60);   break;
					case 1:  c = RGB565(140, 140, 160); break;
					default: c = WHITE; break;
				}
				LCD_Display_Color(sx, sy, sx, sy, c);
				if(stars[i].brightness == 2 && stars[i].speed >= 3) {
					LCD_Display_Color(sx + 1, sy, sx + 1, sy, RGB565(80, 80, 100));
				}
			}
		}

		delay_ms(20);
	}
}

/* ============ 动画5: 旋转方块 (用 LCD_Display_Color 画短粗线段) ============ */
static void anim_spinning_cube(uint16_t duration_frames)
{
	/* 3D立方体顶点 (归一化 -1000 ~ 1000) */
	static const int16_t cube_v[8][3] = {
		{-1000,-1000,-1000}, { 1000,-1000,-1000},
		{ 1000, 1000,-1000}, {-1000, 1000,-1000},
		{-1000,-1000, 1000}, { 1000,-1000, 1000},
		{ 1000, 1000, 1000}, {-1000, 1000, 1000}
	};
	static const uint8_t cube_e[12][2] = {
		{0,1},{1,2},{2,3},{3,0},
		{4,5},{5,6},{6,7},{7,4},
		{0,4},{1,5},{2,6},{3,7}
	};

	uint16_t f;
	int16_t ang_y = 0, ang_x = 0;

	for(f = 0; f < duration_frames; f++) {
		int16_t i;
		int16_t proj[8][2];

		LCD_Clear(BLACK);

		/* 旋转 + 投影 */
		for(i = 0; i < 8; i++) {
			int16_t x = cube_v[i][0];
			int16_t y = cube_v[i][1];
			int16_t z = cube_v[i][2];
			int16_t x1 = (x * icos(ang_y) - z * isin(ang_y)) / 1000;
			int16_t z1 = (x * isin(ang_y) + z * icos(ang_y)) / 1000;
			int16_t y1 = (y * icos(ang_x) - z1 * isin(ang_x)) / 1000;
			int16_t z2 = (y * isin(ang_x) + z1 * icos(ang_x)) / 1000;
			int16_t scale = 5000 / (5000 + z2);
			proj[i][0] = SCREEN_CX + (int16_t)((int32_t)x1 * scale * 60 / 100000);
			proj[i][1] = SCREEN_CY + (int16_t)((int32_t)y1 * scale * 60 / 100000);
		}

		/* 画边: 用少量 LCD_Display_Color 填充 3x3 方块模拟粗线段 */
		for(i = 0; i < 12; i++) {
			int16_t x0 = proj[cube_e[i][0]][0];
			int16_t y0 = proj[cube_e[i][0]][1];
			int16_t x1p = proj[cube_e[i][1]][0];
			int16_t y1p = proj[cube_e[i][1]][1];
			int16_t steps, s;
			int16_t dx = x1p - x0;
			int16_t dy = y1p - y0;
			int16_t ax = dx < 0 ? -dx : dx;
			int16_t ay = dy < 0 ? -dy : dy;
			uint16_t color = rainbow[i % 12];

			steps = ax > ay ? ax : ay;
			if(steps < 4) steps = 4;

			/* 每隔几步画一个 3x3 点, 减少调用次数 */
			for(s = 0; s <= steps; s += 3) {
				int16_t px = x0 + (dx * s) / steps;
				int16_t py = y0 + (dy * s) / steps;
				if(px >= 1 && px < SCREEN_W - 1 && py >= 1 && py < SCREEN_H - 1)
					LCD_Display_Color(px - 1, py - 1, px + 1, py + 1, color);
			}
			/* 确保终点画上 */
			if(x1p >= 1 && x1p < SCREEN_W - 1 && y1p >= 1 && y1p < SCREEN_H - 1)
				LCD_Display_Color(x1p - 1, y1p - 1, x1p + 1, y1p + 1, color);
		}

		/* 画顶点 */
		for(i = 0; i < 8; i++) {
			int16_t px = proj[i][0];
			int16_t py = proj[i][1];
			if(px >= 2 && px < SCREEN_W - 2 && py >= 2 && py < SCREEN_H - 2)
				LCD_Display_Color(px - 2, py - 2, px + 2, py + 2, YELLOW);
		}

		ang_y = (ang_y + 5) % 360;
		ang_x = (ang_x + 3) % 360;
		delay_ms(30);
	}
}

/* ============ 动画6: 脉冲波纹 (用矩形环代替逐像素圆) ============ */
static void anim_ripple(uint16_t duration_frames)
{
	uint16_t f;
	int16_t rings[4];
	int16_t ring_age[4];
	uint16_t ring_i = 0;
	int16_t i;

	memset(rings, 0, sizeof(rings));
	memset(ring_age, 0, sizeof(ring_age));

	LCD_Clear(BLACK);

	for(f = 0; f < duration_frames; f++) {
		/* 每隔 20 帧触发新波纹 */
		if(f % 20 == 0) {
			rings[ring_i] = 4;
			ring_age[ring_i] = 1;
			ring_i = (ring_i + 1) % 4;
		}

		/* 每帧刷新: 先擦除旧环再画新环 */
		for(i = 0; i < 4; i++) {
			if(ring_age[i] <= 0) continue;

			{
				int16_t r = rings[i];
				int16_t x0 = SCREEN_CX - r;
				int16_t y0 = SCREEN_CY - r;
				int16_t x1 = SCREEN_CX + r;
				int16_t y1 = SCREEN_CY + r;
				uint16_t intensity;
				uint16_t c;

				if(r > 5) {
					int16_t ro = r;
					int16_t ri = r - 3;
					/* 擦除外环: 4 条边 */
					LCD_Display_Color(SCREEN_CX - ro, SCREEN_CY - ro, SCREEN_CX + ro, SCREEN_CY - ro, BLACK);
					LCD_Display_Color(SCREEN_CX - ro, SCREEN_CY + ro, SCREEN_CX + ro, SCREEN_CY + ro, BLACK);
					LCD_Display_Color(SCREEN_CX - ro, SCREEN_CY - ro, SCREEN_CX - ro, SCREEN_CY + ro, BLACK);
					LCD_Display_Color(SCREEN_CX + ro, SCREEN_CY - ro, SCREEN_CX + ro, SCREEN_CY + ro, BLACK);
				}

				/* 计算衰减颜色 */
				if(ring_age[i] > 60) {
					intensity = 0;
				} else {
					intensity = 60 - ring_age[i];
				}
				c = RGB565(0, intensity, intensity * 2 > 255 ? 255 : intensity * 2);

				if(intensity > 0) {
					/* 画矩形环 (4 条边) */
					LCD_Display_Color(x0, y0, x1, y0, c);
					LCD_Display_Color(x0, y1, x1, y1, c);
					LCD_Display_Color(x0, y0, x0, y1, c);
					LCD_Display_Color(x1, y0, x1, y1, c);
				}
			}

			rings[i] += 3;
			ring_age[i]++;

			/* 超出屏幕后标记为不活跃 */
			if(rings[i] > 170) {
				ring_age[i] = 0;
			}
		}

		/* 中心点 */
		LCD_Display_Color(SCREEN_CX - 2, SCREEN_CY - 2, SCREEN_CX + 2, SCREEN_CY + 2, WHITE);

		delay_ms(25);
	}
}

/* ============ 动画标题页 ============ */
static void show_title(const char *title, uint16_t color)
{
	LCD_Clear(BLACK);
	LCD_ShowString(0, 40, "========================", GRAY, BLACK, 16);
	LCD_ShowString(0, 100, title, color, BLACK, 32);
	LCD_ShowString(0, 180, "========================", GRAY, BLACK, 16);
	delay_ms(800);
}

/* ============ 主入口 ============ */
void Animation_Demo(void)
{
	srand(12345); /* 固定种子使动画可重复 */

	while(1)
	{
		/* 动画1: 弹跳小球 */
		show_title("Bouncing Balls", GREEN);
		anim_bouncing_balls(300);

		/* 动画2: 雷达扫线 */
		show_title("Radar Sweep", GREEN);
		anim_radar_sweep(500);

		/* 动画3: 彩虹扫屏 */
		show_title("Color Sweep", YELLOW);
		anim_color_sweep(200);

		/* 动画4: 星空 */
		show_title("Starfield", WHITE);
		anim_starfield(400);

		/* 动画5: 旋转立方体 */
		//show_title("3D Cube", CYAN);
		//anim_spinning_cube(300);

		/* 动画6: 脉冲波纹 */
		show_title("Ripple", CYAN);
		anim_ripple(400);
	}
}
