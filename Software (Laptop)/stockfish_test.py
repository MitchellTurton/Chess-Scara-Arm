from stockfish import Stockfish


if __name__ == "__main__":
    print("Stockfish test")
    stockfish = Stockfish()

    stockfish.set_fen_position("rnbqkbnr/pppp1ppp/4p3/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2")

    print(stockfish.get_fen_position())
