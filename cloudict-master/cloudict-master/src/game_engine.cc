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

#include "game_engine.h"
#include "tools.h"


CGameEngine::CGameEngine() {
    strcpy(m_engine_name, "Cloudict.Connect6");
}//引擎对象创建时自动执行，给引擎名称设置默认值 Cloudict.Connect6；如果启动时命令行传了自定义名称，init() 会覆盖这个默认字符串。

int CGameEngine::init(char* name) {

    if (name != NULL && strlen(name) > 0) {
        if (strlen(name) < MSG_LENGTH) {
            strcpy(m_engine_name, name);
        }else {
            printf("To long Engine Name: %s, should less than: %d\n",
                    name, MSG_LENGTH);
        }
    }

    m_alphabeta_depth = 6;
    m_vcf = true;

    m_vcf_search.init(); // VCF engine init the dfa, only once when engine started.
    init_game();

    return 0;
}

void CGameEngine::init_game() {
    init_board(m_board);
    m_vcf_search.init_game();
}

void CGameEngine::on_help() {
    printf(
        "On help for GameEngine %s\n\
            name        - print the name of the Game Engine.\n\
            print       - print the board.\n\
            exit/quit   - quit the game.\n\
            black XXXX  - place the black stone on the position XXXX in the board.\n\
            white XXXX  - place the write stone on the XXXX in the board, X is the A-S.\n\
            next        - the engine will search the move for next step.\n\
            move XXXX   - tell the engine that the opponent take the move XXXX,\n\
                            and the engine will search the move for next step.\n\
            new black   - start a new game and set the engine to Black player.\n\
            new white   - start a new game and set it to White.\n\
            depth d     - set the alpha beta search depth, default is 6.\n\
            vcf         - set vcf search.\n\
            unvcf       - set none vcf search.\n\
            help        - print this help.\n", m_engine_name);
}

int CGameEngine::run()
{
    char msg[MSG_LENGTH] = {0};// 命令输入缓冲区

    on_help();
    while (1)
    {
        // Take in the commands.
        memset(msg, 0, sizeof(msg));// 1. 清空缓冲区 + 读取输入 + 记录日志
        get_msg(msg, MSG_LENGTH);//2.读取新输入，覆盖 msg 数组；
        log_to_file(msg);//把刚读到的新内容写入日志。
        if (strcmp(msg,"name") == 0)
        {
            //name
            printf("name %s\n", m_engine_name);
            fflush(stdout);//强制刷新输出缓冲区。C 语言printf默认是行缓冲，很多时候输出不会立刻显示（尤其是对接对战平台的管道通信时），必须手动刷新才能让对方立刻收到消息，这是棋类引擎的标准写法。
            continue;
        } else
        if (strcmp(msg, "exit") == 0 
                || strcmp(msg, "quit") == 0)
        {
            break;
        } else
        if (strcmp(msg, "print") == 0)
        {
            print_board(m_board, &m_best_move);//调用工具函数 print_board，把 21×21 棋盘以文本形式打印到控制台，通常会高亮标记上一步落子位置（m_best_move）。
        } else if (strcmp(msg, "vcf") == 0) {
	    m_vcf = true;
	} else if (strcmp(msg, "unvcf") == 0) {
	    m_vcf = false;
	} else
        if (strncmp(msg,"black", 5) == 0)//strncmp 匹配前缀
        {
            // Set the position by black stone
            msg2move(&msg[6], &m_best_move);//坐标转换函数，把字母格式的坐标（如 JJ、A1）转换成棋盘数组的 x,y 整数坐标，存入 m_best_move 结构体。
            make_move(m_board, &m_best_move, BLACK);//在棋盘数组 m_board 的对应位置，落下指定颜色的棋子。
            m_chess_type = BLACK;
            continue;
        } else
        if (strncmp(msg,"white", 5) == 0)
        {
            // Set the position by white stone
            msg2move(&msg[6], &m_best_move);
            make_move(m_board, &m_best_move, WHITE);
            m_chess_type = WHITE;
            continue;
        } else
        if (strcmp(msg,"next") == 0)
        {
            m_chess_type = m_chess_type ^ 3;//1 ^ 3 = 2（黑变白），2 ^ 3 = 1（白变黑）
            if (search_a_move(m_chess_type, &m_best_move))//AI 通过算法计算出最优落子；
            {
                make_move(m_board, &m_best_move, m_chess_type);
                strcpy(msg, "move ");
                move2msg(&m_best_move, &msg[5]);
                printf("%s\n",msg);
		        fflush(stdout);
            }
            continue;
        } else
        if (strncmp(msg,"new", 3) == 0)
        {
            //New game.
            init_game();// Begin a new game.
            if (strcmp(&msg[4],"black") == 0)
            {
                //new black
                msg2move("JJ", &m_best_move);
                make_move(m_board, &m_best_move, BLACK);
                m_chess_type = BLACK;
                printf("move JJ\n");
                fflush(stdout);
                continue;
            }
            else
            {
                //new white
                m_chess_type = WHITE;
                continue;
            }

            continue;
        } else
        if (strncmp(msg,"move", 4) == 0)
        {
            //move
            msg2move(&msg[5], &m_best_move);
            make_move(m_board, &m_best_move, m_chess_type^3);
            if (is_win_by_premove(m_board, &m_best_move))
            {
                printf("We loss!");
                fflush(stdout);
                continue;
            }
            if (search_a_move(m_chess_type,&m_best_move))
            {
                // Search for a good move and take it.
                move2msg(&m_best_move, &msg[5]);
                make_move(m_board, &m_best_move, m_chess_type);
                printf("%s\n",msg);
                fflush(stdout);
            }
        } else
        if (strncmp(msg, "depth", 5) == 0) {
            fflush(stdin);
            fflush(stdout);
            int d = 0;
            sscanf(&msg[5], "%d", &d);
            if (d > 0 && d < 10) {
                m_alphabeta_depth = d;
            }
            fflush(stdout);
            printf("Set the search depth to %d.\n\n", m_alphabeta_depth);
            fflush(stdout);
        } else
        if (strncmp(msg, "help", 4) == 0) {
            on_help();
        }
    }
    return 0;
}

bool CGameEngine::search_a_move(char ourColor,move_t* bestMove)
{
    double score = 0;//接收 Alpha-Beta 搜索返回的局面评估分数，分数越高代表对我方越有利。
    clock_t start,end;//记录搜索开始的 CPU 时钟数，用于后续计算耗时。

    // VCF Search first, for the position.
    if (m_vcf) {
        bool vcf = false;//标记本次搜索是否找到必胜走法。
	start = clock();
	m_vcf_search.before_search(m_board, m_chess_type);//VCF 搜索模块的预处理函数，把当前棋盘状态、我方颜色同步给 VCF 模块，重置内部缓存、节点计数器等状态，为正式搜索做准备。
	vcf = m_vcf_search.vcf_search(0,ourColor,bestMove,bestMove,0,-1);//这是 VCF 算法的递归入口，
	end = clock();

	printf("VCF time : %.3lf\n", (double)(end - start)/CLOCKS_PER_SEC);
	// Check if wins.
	if (vcf) {
	    printf("Win by VCF.\n");
	    return true;
	}

	printf("VCF node : %d\n\n", m_vcf_search.m_vcf_node);
    }


    // Alpha beta search continue, if VCF search failed. Alpha-Beta 剪枝全局搜索
    start = clock();
    m_search_engine.before_search(m_board, m_chess_type, m_alphabeta_depth);//把当前棋盘、执棋颜色、搜索深度同步给通用搜索引擎，重置总节点计数器、置换表等内部状态。
    score = m_search_engine.alpha_beta_search(m_alphabeta_depth,MININT,MAXINT,ourColor,bestMove,bestMove);//这是博弈 AI 最经典的Alpha-Beta 剪枝极小极大搜索，
    end = clock();

    printf("AB Time :\t%.3lf\n",(double)(end - start)/CLOCKS_PER_SEC);
    //printf("Get Moves Time :\t%.3lf\n", m_time_get_moves);
    //printf("Set Score Time :\t%.3lf\n", m_time_set_score);
    //printf("Test Time :\t%.3lf\n", m_time_test);
    //printf("Evaluate Time :\t%.3lf\n", m_time_evalution);
    printf("Node :\t%d\n\n", m_search_engine.m_total_nodes);
    printf("Score :\t%.3lf\n",score);

    return true;
}


