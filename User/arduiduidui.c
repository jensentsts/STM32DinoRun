#include "Delay.h"
#include "OLED.h"
#include "ctweeny.h"
#include "img.h"
#include "stm32f10x.h" // Device header
#include "tools.h"

#define ground_y 48

#define dino_jump_max_height 16
#define dino_jump_during 30

#define dino_life 5
#define dino_w 12
#define dino_h 12
#define dino_left 24

#define cloud_max_amount 5

#define block_w 8
#define block_h 8
#define block_max_amount 2

#define game_pause_counter_continue 128

typedef struct
{
    int x, y;
    int w, h;
    int r;
} Cloud;

typedef struct
{
    int x, y;
} Block;

typedef struct
{
    int x, y;
} Bird;

Cloud clouds[cloud_max_amount];
int cloud_amo;
Block blocks[2];
int block_amo;
Bird bird;

int dino_y, dino_jump, dino_lie;

ctweeny dino_jump_ctw;

int dino_alive;

unsigned int score;

int game_pause_counter;

void timer_init()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    // 定时器初始化
    TIM_InternalClockConfig(TIM3); // 选择内部时钟（定时器上电后默认用内部时钟,可省略）
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // 指定时钟分频 - 滤波器的参数 && 信号延迟 && 极性
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 计数器模式（向上计数）
    TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;                 // 自动重装器的值（“周期”），-1由公式得来
    TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;             // 预分频器，-1由公式得来（在 10KHz下记1w个数）
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            // 重复计数器
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

    TIM_ClearFlag(TIM3, TIM_FLAG_Update); // 手动清除更新中断标志位

    // 使能中断
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // 开启更新中断到NVIC的通路
    // NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;           // 选择中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 中断通道是使能还是失能
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 响应优先级
    NVIC_Init(&NVIC_InitStructure);
    // 启动定时器
    TIM_Cmd(TIM3, ENABLE);
}

void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);

    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;          // 选择中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 中断通道是使能还是失能
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        // 响应优先级
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;          // 选择中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 中断通道是使能还是失能
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        // 响应优先级
    NVIC_Init(&NVIC_InitStructure);
}

void ground_draw()
{
    uint8_t i;
    OLED_Line(0, ground_y, 127, ground_y, WHITE);
    // 显示生命值
    for (i = 0; i <= dino_alive; ++i)
    {
        OLED_Square(12 * i - 8, ground_y + 4, 12 * i, ground_y + 4 + 8, 1, WHITE);
    }
}

void cloud_draw()
{
    uint8_t i;
    for (i = 0; i < cloud_amo; ++i)
    {
        OLED_FilletMatrix(clouds[i].x, clouds[i].y, clouds[i].x + clouds[i].w, clouds[i].y + clouds[i].h, clouds[i].r, 0, WHITE);
    }
    OLED_Circle(8, 8, 8, 0, WHITE);
}

void blocks_draw()
{
    uint8_t i;
    for (i = 0; i < block_amo; ++i)
    {
        OLED_Square(blocks[i].x - block_w / 2, ground_y - blocks[i].y - block_h, blocks[i].x + block_w / 2, ground_y - blocks[i].y, 0, WHITE);
    }
}

void bird_draw()
{
}

void dino_draw()
{
    OLED_Square(dino_left, ground_y - dino_y - dino_h / (dino_lie + 1), dino_left + dino_w, ground_y - dino_y, 1, WHITE);
    if (!dino_alive)
    {
        OLED_ShowString(0, 24, " -- Game Over -- ", BLACK);
    }
}

void score_draw()
{
    OLED_ShowNum(87, 0, score, 5, BLACK);
}

void cloud_update()
{
    uint8_t i, j;
    for (i = 0; i < cloud_amo; ++i)
    {
        clouds[i].x--;
    }
    for (i = 0; i < cloud_amo; ++i)
    {
        if (clouds[i].x + clouds[i].w < 0)
        {
            for (j = i + 1; j < cloud_amo; ++j)
            {
                clouds[j - 1] = clouds[j];
            }
            cloud_amo--;
        }
    }
    if (rand() % 16 == 2 && cloud_amo < cloud_max_amount)
    {
        clouds[cloud_amo].x = 127;
        clouds[cloud_amo].y = rand() % 6;
        clouds[cloud_amo].w = 16 + (rand() % 16);
        clouds[cloud_amo].h = 4 + (rand() % 12);
        clouds[cloud_amo].r = 2 + (rand() % 4);
        cloud_amo++;
    }
}

void block_ease_front()
{
    uint8_t j;
    for (j = 1; j < block_amo; ++j)
    {
        blocks[j - 1] = blocks[j];
    }
    block_amo--;
}

void block_update()
{
    uint8_t i, j;
    for (i = 0; i < block_amo; ++i)
    {
        blocks[i].x--;
    }
    for (i = 0; i < block_amo; ++i)
    {
        if (blocks[i].x + 8 < 0)
        {
            block_ease_front();
        }
    }
    if (rand() % 128 <= 2 && block_amo < block_max_amount)
    {
        if (block_amo > 0)
        {
            if (blocks[0].x >= 120)
            {
                return;
            }
        }
        blocks[block_amo].x = 127;
        block_amo++;
    }
}

void dino_update()
{
    uint8_t i;
    if (dino_jump)
    {
        dino_y = ctweeny_step(&dino_jump_ctw, 1);
        if (dino_y == dino_jump_max_height)
        {
            ctweeny_backward(&dino_jump_ctw);
        }
        if (dino_y == 0 && ctweeny_direction(&dino_jump_ctw) == backward)
        {
            dino_jump = 0;
        }
    }
    // 碰撞判断
    for (i = 0; i < block_amo; ++i)
    {
        if (blocks[i].x < dino_left + dino_w &&
            blocks[i].x > dino_left &&
            dino_y < blocks[i].y + block_h)
        {
            dino_alive--;
            block_ease_front();
        }
    }
}

void game_start()
{
    dino_y = 0;
    dino_alive = dino_life;
    ctweeny_init(&dino_jump_ctw, 0, dino_jump_max_height, dino_jump_during);
    ctweeny_via(&dino_jump_ctw, ctweeny_easeOutCubic);
}

void game_restart()
{
    cloud_amo = 0;
    block_amo = 0;
    dino_y = 0;
    score = 0;
    dino_alive = dino_life;
    ctweeny_init(&dino_jump_ctw, 0, dino_jump_max_height, dino_jump_during);
    ctweeny_via(&dino_jump_ctw, ctweeny_easeOutCubic);
}

void game_update()
{
    if (dino_alive)
    {
        cloud_update();
        block_update();
        dino_update();
        ++score;
    }
    else
    {
        game_pause_counter++;
    }
}

void key1_onPress()
{
    if (!dino_jump && dino_alive)
    {
        dino_jump = 1;
        dino_lie = 0;
        ctweeny_foreward(&dino_jump_ctw);
        ctweeny_from(&dino_jump_ctw, dino_y);
        ctweeny_to(&dino_jump_ctw, dino_jump_max_height);
        ctweeny_during(&dino_jump_ctw, dino_jump_during);
    }
    if (!dino_alive)
    {
        if (game_pause_counter >= game_pause_counter_continue)
        {
            game_restart();
        }
    }
}

void key2_onPress()
{
    if (dino_jump)
    {
        ctweeny_during(&dino_jump_ctw, dino_jump_during / 2);
    }
    else
    {
        dino_lie = !dino_lie;
    }
    if (!dino_alive)
    {
        if (game_pause_counter >= game_pause_counter_continue)
        {
            game_restart();
        }
    }
}

void draw()
{
    ground_draw();
    cloud_draw();
    blocks_draw();
    dino_draw();
    score_draw();
}

void setup()
{
    OLED_Init();
    Key_Init();
    timer_init();
    game_start();
}

void loop()
{
    draw();
    OLED_Refresh();
}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {
        game_update();
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) == SET)
    {
        key1_onPress();
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line1) == SET)
    {
        key2_onPress();
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}
