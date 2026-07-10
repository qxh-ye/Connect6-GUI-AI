/*
 * Copyright (c) 2008-2013 Hao Cui <Hao.Cui@Tufts.edu>,
 *                         Liang Li <liliang010@gmail.com>,
 *                         Ruijian Wang <jeoygin@gmail.com>,
 *                         Siran Lin <linsiran@gmail.com>.
 *                         All rights reserved.
 *
 * This program is a free software; you can redistribute it and/or modify
 * it under the terms of the BSD license. See LICENSE.txt for details.
 *
 * Date: 2013/11/01
 *着法生成器 —— AI 的「候选走法筛选器」
 */

#ifndef MOVEGENERATER_H_
#define MOVEGENERATER_H_

#include "defines.h"

class CSearchEngine;

class CMoveGenerator {// AI 提升效率的核心组件
    public:
        CMoveGenerator();
/**
 * 输入：当前执棋颜色 ourColor、棋盘数组 board
输出：把所有候选走法存入 moveList 数组
返回值：生成的候选走法总数量
 */
        int get_move_list(char ourColor , move_t* moveList, char board[][GRID_NUM]);
//这些都是实现细节，分成三大类：点位筛选、点位打分、走法组合排序。
    private:
        int init_valuable_space(char board[][GRID_NUM]);//初始化「有价值空位地图」。扫描整个棋盘，把所有棋子附近的空位标记到内部的 map 数组里，离棋子很远的空位直接忽略
        int sort_merge(move_one_t list[],move_one_t listOne[],int oneN,move_one_t listTwo[],int twoN);//把两组单点列表，两两组合成完整的一步棋（move_t 结构，两个落子点），并按总分从高到低排序。
        bool extend_pos(char x, char y, char board[][GRID_NUM]);//判断某个坐标 (x,y) 是否属于「值得考虑的候选点」。比如检查：是不是空位、周围 1-2 格内有没有棋子，符合条件就返回 true，加入候选列表。
        void add_new_pos_for_two(char x, char y);//普通点位进普通列表；
        void add_new_pos_for_two_special(char x, char y);//special 是特殊关键点位（比如能形成活四、冲四的必胜 / 必防点），进特殊列表，优先级更高，组合走法时会优先考虑。

        int set_score( char ourColor , int step , move_one_t moveList[] , char board[][GRID_NUM]  );//批量打分函数：遍历所有候选单点，逐个计算分数。
        int set_score_single ( char ourColor, int x, int y, int step, char board[][GRID_NUM] );//给单个坐标 (x,y) 计算总分。内部会调用下面四个方向的打分函数，把四个方向的分数加起来，就是这个点的总价值。
        int set_by_direction1 ( char color, int x, int y, int step, char board[][GRID_NUM] );//四个方向分别计算棋型分数，对应棋盘的四个方向：
        int set_by_direction2 ( char color, int x, int y, int step, char board[][GRID_NUM] );
        int set_by_direction3 ( char color, int x, int y, int step, char board[][GRID_NUM] );
        int set_by_direction4 ( char color, int x, int y, int step, char board[][GRID_NUM] );

    public:
        double m_time_get_moves;//生成完整走法列表的总耗时
        double m_time_set_score;
        double m_time_test;

    private:
        int m_dead_four_plus;//棋型相关的常量 / 计数器，一般对应「冲四」这类特殊棋型的分数或标记。
        std::vector<pos_t> m_pos_to_update;//普通候选点位的列表，用 vector 动态数组存。
        std::vector<pos_t> m_pos_to_update_special;//特殊关键点位的列表，优先级更高。

        int map[GRID_NUM][GRID_NUM];//标记地图，和棋盘一样大的二维数组，用来标记每个位置是不是已经被判定为候选点，避免重复计算，提升效率。
};
/**
 * 拿到棋盘 → 过滤掉所有没用的空位，只留棋子附近的点；
给每个候选点，从四个方向算棋型，打出价值分；
把单点两两组合，拼成一步完整的走法（两颗子），按分数排序；
把走法列表交给搜索引擎，让引擎挨个去推演。
 */
#endif

