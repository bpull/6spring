#B.S.Pullig 175148
#SI420 Proj 1 - Fall 2016-2017
import numpy as np

class Board:

    def __init__(self):
        self.b = np.ndarray(shape=(6,7), dtype=(str,11))
        self.b.fill("\033[1;32;40m*")

    def __repr__(self):
        for i in range(6):
            for j in range(7):
                print(str(self.b[i][j]), end="\033[1;32;40m")
            print('')
        print ("\033[1;32;40m1234567")
        return ""

    def play_piece(self, column, token):
        for i in range(5,-1,-1):
            try:
                if self.b[i][column] == "\033[1;32;40m*":
                    if token == 'B':
                        self.b[i][column] = str("\033[1;30;40m") + str(token)
                    else:
                        self.b[i][column] = str("\033[1;31;41m") + str(token)
                    return i
            except Exception as e:
                return False
        return False



class Game:

    def __init__(self):
        self.b = Board()
        self.turn = 'B'
        self.input = ''
        self.over = False

    def next_move(self):
        print (self.b)

        if self.turn == 'R':
            print ("Red player, what's your move?")
        else:
            print ("Black player, what's your move?")
        self.input = input()
        if self.input.isdigit() and int(self.input) != 0:
            self.input = int(self.input) - 1

    def play_turn(self):
        outcome = self.b.play_piece(self.input, self.turn)
        if outcome is not False:
            self.over = self.check_win(outcome)
        else:
            print ("Invalid move! Please try again!")
            self.next_move()
            self.play_turn()

    def check_win(self, row):
        '''check board state for a winning combination. there are only 25 possible straights that can contain 4 in a row'''
        num_in_a_row = 0
        token_counted = '*'
        cur_token = ''

        #check all verticals
        for col in range (7):
            for row in range(6):

                cur_token = self.b.b[row][col]

                if cur_token != '\033[1;32;40m*':
                    if cur_token == token_counted:
                        num_in_a_row = num_in_a_row + 1
                    else:
                        num_in_a_row = 1
                        token_counted = cur_token
                else:
                    num_in_a_row = 0
                    token_counted = '*'

                if num_in_a_row is 4:
                    return True

            num_in_a_row = 0
            token_counted = '\033[1;32;40m*'
            cur_token = ''

        #check all horizontals
        for row in range (6):
            for col in range(7):

                cur_token = self.b.b[row][col]

                if cur_token != '\033[1;32;40m*':
                    if cur_token == token_counted:
                        num_in_a_row = num_in_a_row + 1
                    else:
                        num_in_a_row = 1
                        token_counted = cur_token
                else:
                    num_in_a_row = 0
                    token_counted = '\033[1;32;40m*'

                if num_in_a_row is 4:
                    return True

            num_in_a_row = 0
            token_counted = '\033[1;32;40m*'
            cur_token = ''

        #check all left slanted diagonals
        ranges = [range(2,6), range(1,6), range(6), range(6), range(5), range(4)]
        for cur_range,j in zip(ranges,range(2,-4,-1)):
            for i in cur_range:

                cur_token = self.b.b[i][i-j]

                if cur_token != '\033[1;32;40m*':
                    if cur_token == token_counted:
                        num_in_a_row = num_in_a_row + 1
                    else:
                        num_in_a_row = 1
                        token_counted = cur_token
                else:
                    num_in_a_row = 0
                    token_counted = '\033[1;32;40m*'

                if num_in_a_row is 4:
                    return True

            num_in_a_row = 0
            token_counted = '\033[1;32;40m*'
            cur_token = ''

        #check all right slanted straights
        xranges = [range(3,-1,-1),range(4,-1,-1),range(5,-1,-1),range(5,-1,-1),range(5,0,-1),range(5,1,-1)]
        yranges = [range(4), range(5), range(6), range(1,7), range(2,7), range(3,7)]
        for rows,cols in zip(xranges,yranges):
            for i,j in zip(rows,cols):

                cur_token = self.b.b[i][j]

                if cur_token != '\033[1;32;40m*':
                    if cur_token == token_counted:
                        num_in_a_row = num_in_a_row + 1
                    else:
                        num_in_a_row = 1
                        token_counted = cur_token
                else:
                    num_in_a_row = 0
                    token_counted = '\033[1;32;40m*'

                if num_in_a_row is 4:
                    return True

            num_in_a_row = 0
            token_counted = '\033[1;32;40m*'
            cur_token = ''

        return False

    def change_player(self):
        if self.turn is 'B':
            self.turn = 'R'
        else:
            self.turn = 'B'

    def print_winner(self):
        if self.turn is 'B':
            print("Black Wins!")
        else:
            print("Red Wins!")




if __name__ == '__main__':
    curGame = Game()

    while curGame.over is not True:
        curGame.change_player()
        curGame.next_move()
        curGame.play_turn()

    print(curGame.b)
    print("Game Over!")
    curGame.print_winner()
