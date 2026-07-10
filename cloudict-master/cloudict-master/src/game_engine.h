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

#include "search_engine.h"//封装Alpha-Beta 剪枝博弈搜索算法的 CSearchEngine 类
#include "vcf_search.h" //封装VCF（连续冲四取胜）

class CGameEngine {
    public:
        CGameEngine();
        int init(char* name);       // Init by the engine's name.
        int run();

    private:
        // Init the game board before a game.
        void init_game();
        bool search_a_move(char ourColor,move_t* bestMove);

        void on_help();

    private:
        char                m_board[GRID_NUM][GRID_NUM];        // Game board for Connect6, 19*19 plus 2 borders.
        move_t              m_best_move;                        // The pre best move.
        int                 m_chess_type;                       // The engine's chess color, black or white.
        int                 m_alphabeta_depth;
	bool                m_vcf;//开关变量：true 开启 VCF 杀棋专项搜索，优先判断直接取胜 / 被对手绝杀。

        CSearchEngine       m_search_engine;//内置普通博弈搜索子模块，负责全局最优走法搜索。
        CVCFSearch          m_vcf_search; //内置 VCF 杀棋搜索子模块，专门处理冲四、连续取胜的强制变化。

        char                m_engine_name[MSG_LENGTH];

};

