syntax enable
filetype plugin indent on

autocmd BufNewFile,BufRead *.rs set filetype=rust

let data_dir = has('nvim') ? stdpath('data') . '/site' : '~/.vim'
if empty(glob(data_dir . '/autoload/plug.vim'))
  silent execute '!curl -fLo '.data_dir.'/autoload/plug.vim --create-dirs  https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim'
  autocmd VimEnter * PlugInstall --sync | source $MYVIMRC
endif

" Install vim-plug if not found
if empty(glob('~/.vim/autoload/plug.vim'))
  silent !curl -fLo ~/.vim/autoload/plug.vim --create-dirs
    \ https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim
endif

" Run PlugInstall if there are missing plugins
autocmd VimEnter * if len(filter(values(g:plugs), '!isdirectory(v:val.dir)'))
  \| PlugInstall --sync | source $MYVIMRC
\| endif

call plug#begin('/home/lho/.vim/plugged')

" Plug 'junegunn/fzf', { 'do': { -> fzf#install() } }
Plug 'junegunn/fzf.vim'
Plug 'vim-airline/vim-airline'
Plug 'vim-airline/vim-airline-themes'
Plug 'universal-ctags/ctags'
Plug 'octol/vim-cpp-enhanced-highlight'
Plug 'rust-lang/rust.vim'
Plug 'dense-analysis/ale'

" airline color scheme
Plug 'jacoborus/tender.vim'

call plug#end()

" Fix backspace non sense
noremap! <C-?> <C-h>
set backspace=indent,eol,start

" to resize panes
noremap <C-w>+ :resize +5<CR>
noremap <C-w>- :resize -5<CR>
noremap <C-w>< :vertical:resize -5<CR>
noremap <C-w>> :vertical:resize +5<CR>

" Termdebug
packadd termdebug
let g:termdebug_popup = 0
let g:termdebug_wide = 163

" Status line
if (has("termguicolors"))
 set termguicolors
endif
set termguicolors

" Theme
syntax enable
colorscheme tender
let g:airline_theme='tenderplus'


set hlsearch
hi Search guibg=peru guifg=wheat

" Cursor
set scrolloff=5

" Line numbers
set relativenumber
set number
set number relativenumber
set cursorline

" fixes glitch? in colors when using vim with tmux
let &t_8f = "\<Esc>[38;2;%lu;%lu;%lum"
let &t_8b = "\<Esc>[48;2;%lu;%lu;%lum"

" Tabulations
" set expandtab " enable to replace tab by spaces
set tabstop=4
set softtabstop=0
" set shiftwidth=4

" Rust :

" ALE config 
let g:ale_linters = {
\  'rust': ['analyzer'],
\}

let g:ale_fixers = { 'rust': ['rustfmt', 'trim_whitespace', 'remove_trailing_lines'] }

" Optional, configure as-you-type completions
set completeopt=menu,menuone,preview,noselect,noinsert
let g:ale_completion_enabled = 1

nnoremap <F8> :ALEGoToDefinition<CR>
nnoremap <C-b> : Cbuild<CR>
nnoremap <C-t> : Crun<CR>
nnoremap <C-n> :call ToggleNumbers()<CR>

let g:toggle_numbers = 0

function! ToggleNumbers()
        
    if g:toggle_numbers
        set nu
        set relativenumber
        let g:toggle_numbers = 0
    else
        set nu!
        set norelativenumber
        let g:toggle_numbers = 1
    endif
endfunction

set splitbelow
