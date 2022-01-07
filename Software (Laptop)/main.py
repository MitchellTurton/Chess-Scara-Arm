from chess import ChessGame, WindowManager
from stockfish import Stockfish

chess = ChessGame()
window = WindowManager(800, 800)

chess.set_square_size(window.square_size)

while chess.running:
    window.poll_events(chess)
    window.render_window(chess.board)
