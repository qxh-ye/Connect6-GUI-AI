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
 *
 */

#include "move_generator.h" //着法生成器的头文件
#include "evaluation.h" //局面评估器的头文件，

class CSearchEngine {
    public:
        CSearchEngine();//类实例化时自动调用，用来初始化内部的棋盘、棋子颜色、搜索深度，以及着法生成器、评估器两个组件。

        void before_search(char board[][GRID_NUM], char color, int m_alphabeta_depth); //正式搜索前的准备工作，作用是把外部的对局数据同步到引擎内部：
        double alpha_beta_search(int depth,double alpha,double beta,char ourColor, move_t* bestMove,move_t* preMove);//这是整个 AI 的核心，递归实现极大极小值 +α-β 剪枝的博弈树搜索。
    //alpha / beta：剪枝边界值
    //alpha：己方能接受的最低分数（下界）
    //beta：对方能接受的最高分数（上界）
    //遍历中发现分支超出边界时，直接跳过该分支，大幅减少计算量
    private:

    public:
        int m_total_nodes;//搜索总节点计数，每遍历一个局面就 + 1，用来统计搜索性能，调试时可以查看一次 AI 计算遍历了多少个盘面。

    private:
        char                m_board[GRID_NUM][GRID_NUM];   // The board in the search engine.引擎内部的棋盘副本。搜索时所有的落子、回溯都在这个副本上进行，不会修改外部真实对局的棋盘，保证递归搜索的安全性。
        char                m_chess_type; //AI 己方的棋子颜色（黑 / 白）。评估局面分数时，以这个颜色为基准：对 AI 有利分数为正，不利为负。
        int                 m_alphabeta_depth; //最大搜索深度，决定 AI 能 “往前算多少步”。深度越高棋力越强，但计算量指数级上升。
        CMoveGenerator      m_move_gernerator; //着法生成器实例。每次递归时调用它，生成当前局面所有值得考虑的候选落子，过滤掉无价值的空位，是搜索效率的关键。
        CEvaluation         m_evaluator;//局面评估器实例。搜索到最底层（depth=0）时调用它，给当前盘面打出一个分数，作为递归回溯的评判依据。

};

