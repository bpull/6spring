#!/usr/bin/python3

# SI 335 Algorithms, Spring 2016
# Project 3 Helper code
#
# This program will display the given map and either play a "movie" of the
# moves given, or allow some "live" interaction if there is no moves file
# given.

import argparse
import sys
import curses
import time
import collections

def runmoves(stdscr, world, movesIter, speed, autostop):
    def showMessage(text):
        stdscr.move(msgline, 0)
        stdscr.clrtoeol()
        stdscr.addstr(text, curses.color_pair(4)|curses.A_BOLD)

    def showScore(curscore):
        stdscr.move(*scorePos)
        stdscr.clrtoeol()
        stdscr.addstr(str(curscore), curses.color_pair(4))

    curses.curs_set(0)
    curses.use_default_colors()
    stdscr.nodelay(1)

    curses.init_pair(1, curses.COLOR_BLACK, curses.COLOR_WHITE) # border, obstacles
    curses.init_pair(2, curses.COLOR_YELLOW, curses.COLOR_BLUE) # assets
    curses.init_pair(3, curses.COLOR_MAGENTA, curses.COLOR_WHITE) # targets
    curses.init_pair(4, -1, -1)
    curses.init_pair(5, curses.COLOR_YELLOW, curses.COLOR_RED) # dead assets

    mazedims = world.rows+2, world.cols+2
    world.display(stdscr)
    
    scoreline = mazedims[0] + 1
    stdscr.move(scoreline, 0)
    stdscr.addstr("Score: ", curses.color_pair(4))
    scorePos = stdscr.getyx()

    showScore(world.score)

    infoline = mazedims[0] + 4
    stdscr.move(infoline,0)
    stdscr.addstr("UP/DOWN keys change speed, P pauses/resumes, and Q quits.\n")

    msgline = mazedims[0] + 2

    stdscr.refresh()

    messages = collections.deque()
    paused = False
    if speed < 0:
        paused = True
        speed *= -1

    while True:
        time.sleep(1.0/speed)

        c = stdscr.getch()
        badch = False
        if c == curses.KEY_UP:
            speed += 1
            paused = False
        elif c == curses.KEY_DOWN:
            if speed <= 2:
                speed /= 2
            else:
                speed -= 1
            continue
        elif c == ord('p') or c == ord('P'):
            paused = not paused
        elif c == ord('q') or c == ord('Q'):
            break
        elif c >= 0:
            badch = True

        if badch:
            showMessage("Unrecognized command")
            stdscr.refresh()
            continue

        if world.active() and not paused and speed > 0:
            try:
                nextMove = next(movesIter)
            except StopIteration:
                nextMove = None
                active = False
        else:
            nextMove = None

        if nextMove:
            msgs, status = world.move(nextMove)
            messages.extend(msgs)

            showScore(world.score)
            if status == 0:
                # neutral
                curses.init_pair(4, -1, -1)
            elif status == 1:
                # bad
                curses.init_pair(4, curses.COLOR_RED, curses.COLOR_BLACK)
            elif status == 2:
                # good
                curses.init_pair(4, curses.COLOR_CYAN, curses.COLOR_BLACK)

            if not world.active():
                messages.append("All targets acquired or assets destroyed.")
        if messages:
            showMessage(messages.popleft())

        world.display(stdscr)
        stdscr.refresh()
        
        if not world.active() and not messages and autostop:
            time.sleep(5)
            break

class World:
    def __init__(self, mapin):
        self.rows, self.cols = map(int, next(mapin).split())
        next(mapin) # blank line

        # obstacles is a set of locations
        self.obstacles = set()
        nobst = int(next(mapin))
        for _ in range(nobst):
            r, c = map(int, next(mapin).split())
            self.obstacles.add((r,c))
        next(mapin) # blank line

        self.nasset = int(next(mapin))
        # assets is a list of locations
        self.assets = []
        for _ in range(self.nasset):
            r, c = map(int, next(mapin).split())
            self.assets.append([r,c])
        next(mapin) # blank line

        ntarg = int(next(mapin))
        # targets is a map of label to location/value tuple
        self.targets = {}
        # targloc is a map of location to target label
        self.targloc = {}
        for label in map(chr, range(ord('A'), ord('A') + ntarg)):
            r, c, v = map(int, next(mapin).split())
            self.targets[label] = (r,c,v)
            self.targloc[r,c] = label

        # how many moves have occurred
        self.elapsed = 0

        # current accumulated points
        self.score = 0

        # which assets are dead
        self.dead = set()

        # which spaces have been visited already
        self.trail = set()

    def astr(self, i):
        """Gets a 1-character string from asset index."""
        res = str(i+1)
        if len(res) != 1:
            res = 'x'
        return res

    def display(self, win, offset=(0,0)):
        """Displays the world map at specified offset in specified window."""

        # toshow is a map of location -> (character, style)
        toshow = {}
        for r,c in self.trail:
            toshow[r,c] = (curses.ACS_CKBOARD, curses.color_pair(2)|curses.A_DIM)
        for r,c in self.obstacles:
            toshow[r,c] = (curses.ACS_DIAMOND, curses.color_pair(1))
        for label, (r,c,_) in self.targets.items():
            toshow[r,c] = (label, curses.color_pair(3))
        for i,[r,c] in enumerate(self.assets):
            color = 5 if i in self.dead else 2
            toshow[r,c] = (self.astr(i), curses.color_pair(color)|curses.A_BOLD)

        # top border
        win.addch(offset[0], offset[1], curses.ACS_ULCORNER, curses.color_pair(1))
        for c in range(self.cols):
            win.addch(offset[0], offset[1]+1+c, curses.ACS_HLINE, curses.color_pair(1))
        win.addch(offset[0], offset[1]+1+self.cols, curses.ACS_URCORNER, curses.color_pair(1))

        # the map
        for r in range(self.rows):
            # left border
            win.addch(offset[0]+1+r, offset[1], curses.ACS_VLINE, curses.color_pair(1))
            
            for c in range(self.cols):
                if (r,c) in toshow:
                    char, style = toshow[r,c]
                    win.addch(offset[0]+1+r, offset[1]+1+c, char, style)
                else:
                    win.addch(offset[0]+1+r, offset[1]+1+c, ' ', curses.color_pair(1))

            # right border
            win.addch(offset[0]+1+r, offset[1]+1+self.cols, curses.ACS_VLINE, curses.color_pair(1))

        # bottom border
        win.addch(offset[0]+1+self.rows, offset[1], curses.ACS_LLCORNER, curses.color_pair(1))
        for c in range(self.cols):
            win.addch(offset[0]+1+self.rows, offset[1]+1+c, curses.ACS_HLINE, curses.color_pair(1))
        win.addch(offset[0]+1+self.rows, offset[1]+1+self.cols, curses.ACS_LRCORNER, curses.color_pair(1))

    def move(self, moves):
        """Updates the assets according to the given list of moves.
        Returns a messages/status pair.
        Status is 0 if nothing special, 1 if something bad happened,
        2 if something good happened."""
        assert len(moves) == self.nasset

        msgs = []
        status = 0

        # one more step has been taken
        self.elapsed += 1

        # remove any target values <= 0
        zero_targets = []
        for label, (r,c,v) in self.targets.items():
            if v <= self.elapsed:
                zero_targets.append(label)
        for label in zero_targets:
            self.remove_target(label)

        for i, (dr, dc) in enumerate(moves):
            if i in self.dead:
                # dead assets don't move
                continue

            self.trail.add(tuple(self.assets[i]))
            self.assets[i][0] += dr
            self.assets[i][1] += dc
            r,c = self.assets[i]
            if r < 0 or r >= self.rows or c < 0 or c > self.cols:
                # ran out of bounds
                self.assets[i][0] -= dr
                self.assets[i][1] -= dc
                msgs.append("Asset {} ran out of bounds".format(self.astr(i)))
                self.dead.add(i)
                status = 1
            elif (r,c) in self.obstacles:
                # ran into an obstacle
                msgs.append("Asset {} ran into an obstacle".format(self.astr(i)))
                self.dead.add(i)
                status = 1
            elif (r,c) in self.targloc:
                # we got 'em!
                label = self.targloc[r,c]
                value = self.targets[label][2] - self.elapsed
                assert value > 0
                msgs.append("Asset {} got target {} for {} points".format(self.astr(i),label,value))
                self.remove_target(label)
                self.score += value
                status = 2

        return msgs,status

    def active(self):
        """Returns True only when there are more targets, and some assets are not dead."""
        return len(self.targets) > 0 and len(self.dead) < len(self.assets)

    def remove_target(self, label):
        r,c,v = self.targets[label]
        del self.targets[label]
        del self.targloc[r,c]

def read_moves(movesin, nasset):
    """Returns an iteration of moves. Each move is a tuple of row,col changes."""
    for line in movesin:
        directions = line.strip()
        assert len(directions) == nasset
        move = []
        for d in directions.upper():
            if d == 'U': move.append((-1,0))
            elif d == 'D': move.append((1,0))
            elif d == 'L': move.append((0,-1))
            elif d == 'R': move.append((0,1))
            else:
                raise RuntimeError("invalid move character: "+d)
        yield move

def main():
    # parse command-line arguments
    parser = argparse.ArgumentParser(description="Show a map and associated moves")
    parser.add_argument("mapfile", type=argparse.FileType('r'),
            help="filename of the map")
    parser.add_argument("movesfile", type=argparse.FileType('r'), 
            default=None, nargs='?',
            help="filename of the moves")
    parser.add_argument('speed', type=float, default=2.0, nargs='?',
            help="speed to display the moves, in frames per second. Default is 2")
    parser.add_argument('-a', '--autostop', action='store_true')
    args = parser.parse_args()

    # read in the map
    world = World(args.mapfile)
    
    # read in moves
    if args.movesfile:
        moves = read_moves(args.movesfile, world.nasset)
    else:
        moves = iter([])

    try:
        curses.wrapper(runmoves, world, moves, args.speed, args.autostop)
    except curses.error:
        print("There was an error displaying the map with curses.")
        print("Check that your terminal window is large enough for the map.")
        exit(3)

if __name__ == '__main__':
    main()
