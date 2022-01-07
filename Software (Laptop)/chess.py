import pygame
from stockfish import Stockfish

import math
from typing import List, Tuple

from pieces import Piece, BlankPiece, Pawn, Knight, Bishop, Rook, Queen, King

def get_row(pos: int) -> float:
    """
    Returnes the row of a position
    """

    return math.floor(pos / 8)

def get_file(pos: int) -> float:
    """
    Returns the file of a position
    """
    return pos % 8


class ChessGame:
    """
    Class representing a single instance of a game of chess
    """

    square_size: float  # The size of each square of the chess board

    board: List[Piece]  # Array that holds each Piece class representing positions on the board

    castling_info: List[bool]

    selected_piece: int  # Holds the position of the selected piece

    running: bool

    current_color: int  # Which side is currently playing

    stockfish: Stockfish # chess engine for desciding moves

    ai_color: int

    color_map = {'Colorless': 0, 'Black': -1, 'White': 1}  # Simple dict to make certain pieces of code more readable

    def __init__(self, starting_fenstring:str = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq', ai_color = 'Black') -> None:
        """
        Constructor for the ChessGame class
        """

        self.square_size = 25

        self.board, self.current_color, self.castling_info = self.load_fenstring(starting_fenstring)
        self.highlight_rects = []  # Contains rects to highlight certain squares on the board
        self.history = []

        self.selected_piece = BlankPiece(-1)

        self.running = True
        
        self.stockfish = Stockfish()
        self.stockfish.set_fen_position(starting_fenstring + "- 0 0")
        self.ai_color = self.color_map[ai_color]

        self.gen_legal_moves()

        for piece in self.board:
            piece.scale_sprite(int(self.square_size), int(self.square_size))

    def load_fenstring(self, fenstring:str) -> None:
        """
        Positions pieces on the board based off of a fenstring
        """

        piece_types = {'p': Pawn, 'n': Knight, 'b': Bishop, 'r': Rook, 
                       'q': Queen, 'k': King}

        piece_pos, color_str, castling_str = fenstring.split()

        new_board = []
        row, file = 0, 0

        for char in piece_pos:
            if char == '/':
                row += 1
                file = 0
            elif char.isdigit():
                for i in range(int(char)):
                    new_board.append(BlankPiece(row * 8 + file))
                    file += 1 
            else:
                color = (self.color_map['White'] if char.isupper()
                         else self.color_map['Black'])
                
                piece = piece_types[char.lower()](row * 8 + file, color)
                piece.scale_sprite(int(self.square_size), int(self.square_size))
                
                if fenstring == 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq' and (type(piece) == King 
                        or type(piece) == Rook):
                    piece.can_castle = True

                new_board.append(piece)
                file += 1

        curr_color = 1 if color_str == 'w' else -1

        castling_info:dict[char, bool] = {x:False for x in "KQkq"}

        for c in "KQkq":
            if c in castling_str:
                castling_info[c] = True


        return new_board, curr_color, castling_info

    def stockfish_move(self):
        self.stockfish.set_fenstring(self.get_fenstring())

        move_str = self.stockfish.get_best_move()

        move = move_str.split()

    def move(self, initial_pos:int, final_pos:int) -> None:
        """
        Moves a chess piece
        """
        
        selected_piece:Piece = self.board[initial_pos]
        piece_type = type(selected_piece)

        if (piece_type == Pawn and (get_row(final_pos) == 0 
                or get_row(final_pos) == 7)):
            piece_type = Queen
        
        self.board[final_pos] = piece_type(final_pos, 
                                    selected_piece.color_val)
        self.board[final_pos].scale_sprite(int(self.square_size), 
                                           int(self.square_size))

        self.board[initial_pos] = BlankPiece(initial_pos)

        self.selected_piece = BlankPiece(-1)

        # Moving Rook when castling
        if type(selected_piece) == King and final_pos in selected_piece.castle_moves:
            if final_pos > selected_piece.pos:
                self.move(selected_piece.pos + 3, final_pos - 1)
            else:
                self.move(selected_piece.pos - 4, final_pos + 1)

    def undo_move(self) -> None:
        """
        Undoes a move just made
        
        Has a limit of 10 moves of history
        """
        
        if self.history:

            self.board = self.history[-1]
            del self.history[-1]

            self.update_new_position()
        else:
            print("You can not undo anymore")

    def update_history(self):
        """
        Adds the current state to the history array and removes the first
        state if the limit is exceeded (limit:int = 10)
        """

        self.history.append(self.board.copy())

        if len(self.history) > 10:
            del self.history[0]

    def update_new_position(self) -> None:
        """
        Function that runs directly after a move is made
        """

        self.gen_legal_moves()
        self.current_color *= -1  # Switching current color

        if self.current_color == self.ai_color:
            self.stockfish_move()

    

    def gen_legal_moves(self) -> None:
        """
        Loops over each piece in chess.board and updates what legal moves they 
        can make
        """

        for piece in self.board:
            piece.gen_legal_moves(self.board)

        king_start_pos = [4, 60]
        for i in king_start_pos:
            king = self.board[i]

            if type(king) == King:
                if king.can_castle:
                    king.check_castle(self.board)

    def set_square_size(self, square_size: int) -> None:
        
        self.square_size = square_size

        for piece in self.board:
            piece.scale_sprite(int(self.square_size), int(self.square_size))


class WindowManager:
    """
    Class to manage all the rendering displaying and user input
    """

    win_width: int 
    win_height: int 

    window: pygame.Surface

    square_size: float  # The width/height of across for each tile of the board

    board_rects: List[pygame.Rect]  # List holding colored squares representing the chess board
    
    # Hold the rectangles that are drawn to make up the chessboard.
    # Surface used instead of Rect for transparancy
    highlight_rects: List[Tuple[pygame.Surface, Tuple[int]]]
    highlight_alpha: int  # defines how transparent highlighted squares are

    _light_color: pygame.Color 
    _dark_color: pygame.Color
    _highlight_color: pygame.Color   

    def __init__(self, window_width: int, window_height: int, name: str="Chess") -> None:
        """
        Window Manager constructor
        """
        
        pygame.init()


        self.win_width = window_width
        self.win_height = window_height

        self.window = pygame.display.set_mode((self.win_width, self.win_height))

        pygame.display.set_caption(name)

        self.square_size = self.win_width / 8 

        self.board_rects = [] 

        # Initializing the board_rects array
        for i in range(64):
            row = math.floor(i / 8)
            file = i % 8

            self.board_rects.append(pygame.Rect(file * self.square_size, row
                                                * self.square_size,
                                                self.square_size,
                                                self.square_size))

        self.highlight_rects = []
        self.highlight_alpha = 77  # 77 -> 50% transparancy

        # Colors
        self._light_color = pygame.Color(240, 240, 240)
        self._dark_color = pygame.Color(46, 139, 87)
        self._highlight_color = pygame.Color(255, 0, 0)

        self.chess_game = ChessGame()

    def highlight_legal_moves(self, selected_piece: Piece) -> None:
        """
        Highlights all tiles a selected piece can move to
        """

        self.highlight_rects.clear()

        moves:List[int] = selected_piece.moves

        if type(selected_piece) == King:
            moves += selected_piece.castle_moves

        for move in moves:

            row = get_row(move)
            file = get_file(move)

            rect = pygame.Surface((self.square_size, self.square_size))
            rect.set_alpha(self.highlight_alpha)  # Adding transparancy
            rect.fill(self._highlight_color)
            pos = (file * self.square_size, row * self.square_size)

            self.highlight_rects.append((rect, pos))

    def poll_events(self, curr_game: ChessGame) -> None:
        """
        Takes input from the user
        """

        for event in pygame.event.get():
            
            # Quitting the window
            if event.type == pygame.QUIT:
                curr_game.running = False

            # Mouse input
            if event.type == pygame.MOUSEBUTTONUP:
                
                # Getting position of the mouse
                x, y = pygame.mouse.get_pos()
                row = math.floor(y / self.square_size)
                file = math.floor(x / self.square_size)
                pos = row * 8 + file

                # Selecting where to move a piece (2nd click)
                if pos in curr_game.selected_piece.moves:
                    curr_game.update_history()
                    curr_game.move(curr_game.selected_piece.pos, pos)
                    curr_game.update_new_position()

                    self.highlight_rects.clear()
                # Selecting a piece
                elif curr_game.board[pos].color_val == curr_game.current_color:
                    curr_game.selected_piece = curr_game.board[pos]
                    self.highlight_legal_moves(curr_game.selected_piece)
                else:
                    curr_game.selected_piece = BlankPiece(-1)
                    self.highlight_rects.clear()
            
            # Keyboard input
            if event.type == pygame.KEYUP:
                if event.key == pygame.K_u:
                    curr_game.undo_move()

    def render_window(self, chess_board: List[Piece]) -> None:
        """
        Function containing code to render to the pygame window
        """

        # Drawing the chess board
        for i, rect in enumerate(self.board_rects):
            color = (self._light_color if (get_file(i) + get_row(i))
                     % 2 == 0 else self._dark_color)
            pygame.draw.rect(self.window, color, rect)

        # Highlighting squares
        for rect, pos in self.highlight_rects:
            self.window.blit(rect, pos) 

        # Drawing the chess pieces on the board
        for piece in chess_board:
            if type(piece) != BlankPiece:
                self.window.blit(piece.sprite, (piece.file * self.square_size,
                                piece.row * self.square_size))
       
        pygame.display.update()

if __name__ == '__main__':
    chess = ChessGame()

    chess.load_fenstring("8/8/8/4n3/8/8/8/8")

    while chess.running:
        chess.poll_events()
        chess.gen_legal_moves()
        chess.gen_highlighted()
        chess.render_window()
