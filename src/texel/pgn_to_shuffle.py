import chess
import chess.pgn
import chess.engine
import random

fen_list = []

with open("data.pgn", "r") as pgn_file:
    while True:
        game = chess.pgn.read_game(pgn_file)
        if game is None:
            break
        board = game.board()

        result = game.headers["Result"]
        if (result == "1-0"):
            result = 1
        elif (result == "0-1"):
            result = 0
        else:
            result = 0.5

        for node in game.mainline():
            move = node.move
            board.push(move)
            comment = node.comment
            if "book" not in comment and "+M" not in comment and "-M" not in comment:
                fen_list.append((board.fen(), result))

random.shuffle(fen_list)

with open("fen.txt", "w") as f:
    for fen, result in fen_list:
        f.write(fen + "\n")

with open("result.txt", "w") as f:
    for fen, result in fen_list:
        f.write(str(result) + "\n")
