// total_pieces = shape_temp.length0 + shape_temp.length1;
// total_length = total_pieces + (shape_temp.length1 == 0 ? 0 : 1);
// heat_temp = value_blocked[total_pieces] - value_free[total_pieces - 1];
//
// if (!shape_temp.isblocked_end)
// {
//     //分值表的第0位为一子
//     heat_push({
//                   (char)(point.Y + deltaY * (total_length + 1)),
//                   (char)(point.X + deltaX * (total_length + 1))
//               }, heat_temp);
//     score_temp = player * (value_blocked[total_pieces - 1] - value_free[total_pieces - 2]);
// }
// else if (total_length < 5)
// {
//     heat_temp = -value_blocked[total_pieces - 1];
//     score_temp = -player * value_blocked[total_pieces - 2];
// }
// if (shape_temp.length1 != 0)
// {
//     //中间断开一律算冲四
//
//     heat_push({
//                   (char)(point.Y + deltaY * (shape_temp.length0 + 1)),
//                   (char)(point.X + deltaX * (shape_temp.length0 + 1))
//               }, heat_temp);
//     if (total_length >= 5)
//     {
//         score_temp = player * (value_blocked[4 - 1] - value_blocked[3 - 1]);
//     }
// }
// shape_push(shape_create, {(char)(point.Y + deltaY), (char)(point.X + deltaX)}, shape_temp, dir);
// BoardState[point.Y + deltaY][point.X + deltaX][dir].length0 = 0;
//
// score_push(score_temp);

































//     switch (BoardState[point.Y+deltaY][point.X+deltaX][dir].shape) {
            //         case single:
            //             if (Board[point.Y+deltaY*2][point.X+deltaX*2] == 0) {
            //                 BoardState[point.Y][point.X][dir].shape =neighborx110;
            //                 HeatMap[point.Y+deltaY*2][point.X+deltaX*2] += SCORE_THREE_BLOCKED - SCORE_NEIGHBOR_FREE;
            //                 Score += SCORE_NEIGHBOR_BLOCKED - SCORE_SINGLE;
            //             }
            //             else{
            //                 BoardState[point.Y][point.X][dir].shape =neighbor_dead;
            //                 Score -= SCORE_SINGLE;
            //             }
            //             break;
            //         case neighbor0110:
            //             BoardState[point.Y][point.X][dir].shape =threex1110;
            //             HeatMap[point.Y+deltaY*3][point.X+deltaX*3] += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             Score += SCORE_THREE_BLOCKED - SCORE_NEIGHBOR_FREE;
            //             break;
            //         case neighbor011x:
            //             BoardState[point.Y][point.X][dir].shape =three_dead;
            //             Score -= SCORE_NEIGHBOR_BLOCKED;
            //             break;
            //         case three01110:
            //             BoardState[point.Y][point.X][dir].shape =fourx11110;
            //             HeatMap[point.Y+deltaY*4][point.X+deltaX*4] += SCORE_WIN - SCORE_FOUR_BLOCKED;
            //             Score += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             break;
            //         case three0111x:
            //             BoardState[point.Y][point.X][dir].shape =four_dead;
            //             Score -= SCORE_THREE_BLOCKED;
            //             break;
            //         case four01111x:
            //         case four011110:
            //             BoardState[point.Y][point.X][dir].shape =five;//进出成五均不处理heatmap
            //             Score += SCORE_WIN - SCORE_FOUR_BLOCKED;//这里不对
            //             break;
            //         case jump01010:
            //             BoardState[point.Y][point.X][dir].shape =threejumpx11010;
            //             HeatMap[point.Y+deltaY*2][point.X+deltaX*2] += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             HeatMap[point.Y+deltaY*4][point.X+deltaX*4] += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             Score += SCORE_THREE_BLOCKED - SCORE_NEIGHBOR_FREE;
            //             break;
            //         case jump0101x:
            //             BoardState[point.Y][point.X][dir].shape =threejump1101dead;
            //             HeatMap[point.Y+deltaY*2][point.X+deltaX*2] -=  SCORE_THREE_BLOCKED;
            //             Score -=  SCORE_NEIGHBOR_BLOCKED;
            //             break;
            //         case threejump010110:
            //             BoardState[point.Y][point.X][dir].shape =fourjump11011;
            //             HeatMap[point.Y+deltaY*2][point.X+deltaX*2] += SCORE_WIN - SCORE_FOUR_BLOCKED;
            //             HeatMap[point.Y+deltaY*5][point.X+deltaX*5] += -SCORE_FOUR_BLOCKED;
            //             Score += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             break;
            //         case threejump01011x:
            //             BoardState[point.Y][point.X][dir].shape =fourjump11011;
            //             HeatMap[point.Y+deltaY*2][point.X+deltaX*2] += SCORE_WIN - SCORE_FOUR_BLOCKED;
            //             Score += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             break;
            //         case threejump011010:
            //             BoardState[point.Y][point.X][dir].shape =fourjump11101;
            //             HeatMap[point.Y+deltaY*3][point.X+deltaX*3] += SCORE_WIN - SCORE_FOUR_BLOCKED;
            //             HeatMap[point.Y+deltaY*5][point.X+deltaX*5] += -SCORE_FOUR_BLOCKED;
            //             Score += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             break;
            //         case threejump01101x:
            //             BoardState[point.Y][point.X][dir].shape =fourjump11011;
            //             HeatMap[point.Y+deltaY*3][point.X+deltaX*3] += SCORE_WIN - SCORE_FOUR_BLOCKED;
            //             Score += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             break;
            //     }
            //
            //     BoardState[point.Y+deltaY][point.X+deltaX][dir].shape =null;
            // }
            // else if (Board[point.Y+2*deltaY][point.X+2*deltaX] == player) {
            //     switch (BoardState[point.Y+deltaY][point.X+deltaX][dir].shape) {
            //         case single:
            //             if (Board[point.Y+deltaY*2][point.X+deltaX*2] == 0) {
            //                 BoardState[point.Y][point.X][dir].shape =jumpx1010;
            //                 HeatMap[point.Y+deltaY][point.X+deltaX] += SCORE_THREE_BLOCKED - SCORE_NEIGHBOR_FREE;
            //                 HeatMap[point.Y+deltaY*3][point.X+deltaX*3] += SCORE_THREE_BLOCKED - SCORE_NEIGHBOR_FREE;
            //                 Score += SCORE_NEIGHBOR_BLOCKED - SCORE_SINGLE;
            //             }
            //             else{
            //                 BoardState[point.Y][point.X][dir].shape =jump_dead;
            //                 Score -= SCORE_SINGLE;
            //             }
            //             break;
            //         case neighbor0110:
            //             BoardState[point.Y][point.X][dir].shape =threejumpx10110;
            //             HeatMap[point.Y+deltaY][point.X+deltaX] += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             HeatMap[point.Y+deltaY*4][point.X+deltaX*4] += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             Score += SCORE_THREE_BLOCKED - SCORE_NEIGHBOR_FREE;
            //             break;
            //         case neighbor011x:
            //             BoardState[point.Y][point.X][dir].shape =threejump1011dead;
            //             Score -= SCORE_NEIGHBOR_BLOCKED;
            //             break;
            //         case three01110:
            //             BoardState[point.Y][point.X][dir].shape =fourjump10111;
            //             HeatMap[point.Y+deltaY][point.X+deltaX] += SCORE_WIN - SCORE_FOUR_BLOCKED;HeatMap[point.Y+deltaY*5][point.X+deltaX*5] += -SCORE_FOUR_BLOCKED;
            //             Score += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             break;
            //         case three0111x:
            //             BoardState[point.Y][point.X][dir].shape =four_dead;
            //             Score -= SCORE_THREE_BLOCKED;
            //             break;
            //         case four01111x:
            //         case four011110:
            //             BoardState[point.Y][point.X][dir].shape =five;//进出成五均不处理heatmap
            //             Score += SCORE_WIN - SCORE_FOUR_BLOCKED;//这里不对
            //             break;
            //         case jump01010:
            //             BoardState[point.Y][point.X][dir].shape =threejumpx11010;
            //             HeatMap[point.Y+deltaY*2][point.X+deltaX*2] += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             HeatMap[point.Y+deltaY*4][point.X+deltaX*4] += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             Score += SCORE_THREE_BLOCKED - SCORE_NEIGHBOR_FREE;
            //             break;
            //         case jump0101x:
            //             BoardState[point.Y][point.X][dir].shape =threejump1101dead;
            //             HeatMap[point.Y+deltaY*2][point.X+deltaX*2] -=  SCORE_THREE_BLOCKED;
            //             Score -=  SCORE_NEIGHBOR_BLOCKED;
            //             break;
            //         case threejump010110:
            //             BoardState[point.Y][point.X][dir].shape =fourjump11011;
            //             HeatMap[point.Y+deltaY*2][point.X+deltaX*2] += SCORE_WIN - SCORE_FOUR_BLOCKED;
            //             HeatMap[point.Y+deltaY*5][point.X+deltaX*5] += -SCORE_FOUR_BLOCKED;
            //             Score += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             break;
            //         case threejump01011x:
            //             BoardState[point.Y][point.X][dir].shape =fourjump11011;
            //             HeatMap[point.Y+deltaY*2][point.X+deltaX*2] += SCORE_WIN - SCORE_FOUR_BLOCKED;
            //             Score += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             break;
            //         case threejump011010:
            //             BoardState[point.Y][point.X][dir].shape =fourjump11101;
            //             HeatMap[point.Y+deltaY*3][point.X+deltaX*3] += SCORE_WIN - SCORE_FOUR_BLOCKED;
            //             HeatMap[point.Y+deltaY*5][point.X+deltaX*5] += -SCORE_FOUR_BLOCKED;
            //             Score += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             break;
            //         case threejump01101x:
            //             BoardState[point.Y][point.X][dir].shape =fourjump11011;
            //             HeatMap[point.Y+deltaY*3][point.X+deltaX*3] += SCORE_WIN - SCORE_FOUR_BLOCKED;
            //             Score += SCORE_FOUR_BLOCKED - SCORE_THREE_FREE;
            //             break;
            //     }
            //
// Created by 13578 on 2025/12/8.
//