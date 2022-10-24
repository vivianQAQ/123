"			__     _____ __  __ ____   ____
"			\ \   / /_ _|  \/  |  _ \ / ___|
"			 \ \ / / | || |\/| | |_) | |
"			  \ V /  | || |  | |  _ <| |___
"			   \_/  |___|_|  |_|_| \_\\____| By yang.zisong
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

"Bundle插件管理设置
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
set shell=/bin/bash
filetype off

set rtp+=~/.vim/bundle/vundle
call vundle#begin()

" let Vundle manage Vundle, required
Plugin 'VundleVim/Vundle.vim'

"Plugin 'gmarik/vundle'
set tags=tags;
" My Bundles here:
"vim-scripts repos
Plugin 'L9'
Plugin 'FuzzyFinder'
Plugin 'derekwyatt/vim-scala'

" vim
Plugin 'https://github.com/terryma/vim-multiple-cursors.git'

" lua
Plugin 'https://github.com/xolox/vim-misc.git'
Plugin 'https://github.com/xolox/vim-lua-ftplugin.git'

" tagbar
Plugin 'majutsushi/tagbar.git'
Plugin 'taghighlight'

" nerdtree
Plugin 'https://github.com/scrooloose/nerdtree.git'
Plugin  'https://github.com/mattn/emmet-vim.git'

" previm
Plugin 'kannokanno/previm'
Plugin 'tyru/open-browser.vim'

" git
"Plugin 'wsdjeg/git.vim'

" python format
Plugin 'mindriot101/vim-yapf'

" json
"Plugin 'https://github.com/elzr/vim-json'

" doxygentoolkit
Plugin 'vim-scripts/DoxygenToolkit.vim'

"
"Plugin 'Valloric/YouCompleteMe'

"
"Plugin 'Chiel92/vim-autoformat'

"Plugin 'JamshedVesuna/vim-markdown-preview'
Plugin 'shawncplus/phpcomplete.vim'
Plugin 'beanworks/vim-phpfmt'

" golang
"Plugin 'https://github.com/fatih/vim-go'
Plugin 'https://github.com/Blackrush/vim-gocode'

call vundle#end()

filetype plugin indent on
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

"实用设置
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" 关闭vi的一致性模式，避免以前版本的一些Bug和局限
set nocompatible
set backspace=indent,eol,start
" 共享剪贴板
set clipboard+=unnamed
set clipboard+=unnamedplus
" 修改文自动备份
set nobackup
" 文件在Vim之外修改过，自动重新读入"
set autoread
" 设置自动保存内容"
set autowrite
" 配置改动时自动加载
autocmd! bufwritepost vimrc source ~/.vimrc
" 打开文件自动跳到上次编辑的位置
au BufReadPost * if line("'\'") > 1 && line("'\'") <= line("$") | exe "normal! g'\'" | endif
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

" 编码格式设置
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
set encoding=utf-8
set fileencoding=utf-8
set termencoding=utf-8
set fencs=utf-8,ucs-bom,gb18030,gbk,gb2312,chinese
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

" 基本设置
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" 读法高亮
syntax on
set cc=80
" 设置自动对齐
set autoindent
set cindent
set smartindent
" 自动补全配置让Vim补全菜单行为跟IDE一致
set completeopt=longest,menu
" 增强模式中的命令行自动完成操作
set wildmenu
" 设置鼠标可用
"set mouse=a
"set selection=exclusive
"set selectmode=mouse,key
" 设置tab宽度
set tabstop=4
set expandtab
" 设置自动对齐空格数
set shiftwidth=4
" 历史记录数
set history=1000
" 为特定文件类型载入相关缩进文件
filetype indent on
" 保存全局变量
set viminfo+=!
" 去掉输入错误的提示声音
set noeb
set noswapfile
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

" 显示设置
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" 突出显示当前行
"set cursorline
"set cursorcolumn
" 设置魔术
set magic
" 显示当前行号和列号
set ruler
" 在状态栏显示正在输入的命令
set showcmd
" 左下角显示当前Vim模式
set showmode
" 显示行号
set number
" 指定不折行
"set nowrap
" 设置代码匹配,包括括号匹配情况
 set showmatch
" 设置搜索高亮(hlsearch)
 set hlsearch
"set incsearch
" 设置搜索时忽略大小写
set ignorecase
" 当搜索的时候尝试smart
set smartcase
" 背景设置
"colorscheme solarized
"let g:solarized_termtrans=1	"1为使用Terminal的透明背景
"set background=dark
" 高亮结尾空格
highlight WhitespaceEOL ctermbg=red guibg=red
match WhitespaceEOL /\s\+$/
" 总是显示状态行
set laststatus=2
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

" tab设置
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
"autocmd BufEnter *.am, set filetype=mk
"autocmd BufEnter *.mk, set filetype=mk
autocmd BufEnter *.thrift, set filetype=thrift

autocmd FileType go :set tabstop=4 noexpandtab " Do not use spaces instead of tabs
autocmd FileType lua :set tabstop=4 shiftwidth=4 expandtab ai
autocmd FileType thrift :set tabstop=4 shiftwidth=4 expandtab ai
autocmd FileType automake :set tabstop=4 shiftwidth=4 expandtab ai
autocmd FileType c,cpp :set tabstop=4 shiftwidth=4 expandtab ai
autocmd FileType vimwiki :set tabstop=4 shiftwidth=4 expandtab ai
autocmd FileType python :set tabstop=4 shiftwidth=4 expandtab ai
autocmd FileType ruby,javascript,html,css,xml,vue :set tabstop=2 shiftwidth=2 softtabstop=2 expandtab ai
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

"vimwiki设置
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
syntax on
filetype plugin on
let g:vimwiki_list = [{'path': '~/wiki/Study',
	\ "syntax": "markdown",
	\ 'path_html': '~/wiki/html/',
	\ 'template_path': '~/wiki/template/',
	\ 'template_default': 'template',
	\ 'template_ext': '.html',
	\ 'ext': '.md'},
	\ {'path': '~/wiki/Work',
	\ 'ext': '.md',
	\ "syntax": "markdown",},
	\ {'path': '~/wiki/Life',
	\ "syntax": "markdown",
	\ "ext": '.md'},
	\ {'path': '~/wiki/blog',
	\ 'ext': '.md',
	\ "syntax": "markdown",}
	\]
	"多个wiki项目、\ws显示项目列表,或者2\ww进入第二个项目
let g:vimwiki_camel_case = 1
let g:vimwiki_use_mouse = 1
map <F4> <Plug>Vimwiki2HTML
map <S-F4> <Plug>VimwikiAll2HTML
let g:vimwiki_file_exts = 'c, cpp, wav, txt, h, hpp, zip, sh, awk, ps, pdf'
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

"NERDTree设置
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
"只剩下目录树时则自动关闭目录树
autocmd bufenter * if (winnr("$") == 1 && exists("b:NERDTreeType") && b:NERDTreeType == "primary") | q | endif
"当打开vim且没有文件时自动打开NERDTree
"autocmd vimenter * if !argc() | NERDTree | endif
"autocmd vimenter *  NERDTree
" 让Tree把自己给装饰得多姿多彩漂亮点
let NERDChristmasTree=1
" 控制当光标移动超过一定距离时，是否自动将焦点调整到屏中心
let NERDTreeAutoCenter=1
" 指定鼠标模式(1.双击打开 2.单目录双文件 3.单击打开)
let NERDTreeMouseMode=2
" 是否默认显示书签列表
let NERDTreeShowBookmarks=1
" 是否默认显示文件
let NERDTreeShowFiles=1
" 是否默认显示隐藏文件
let NERDTreeShowHidden=1
" 是否默认显示行号
let NERDTreeShowLineNumbers=0
" 窗口位置（’left’ or ‘right’）
"let NERDTreeWinPos='right'
" 窗口宽度
let NERDTreeWinSize=31
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

"新文件标题设置
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

"新建.py,.c,.h,sh,.Java文件，自动插入文件头
autocmd BufNewFile *.py,*.cpp,*.[ch],*.sh,*.java,*.php exec ":call SetTitle()"
"autocmd BufNewFile *.py exec ":call SetTitle()"
""定义函数SetTitle，自动插入文件头

func SetTitle()
    " 设置用户信息
    let authorName = "yang.zisong"
    let mailAddr = "yzs2054@163.com"
    let versionString = "1.0"
    "如果文件类型为.sh文件
    if &filetype == 'sh'
        call setline(1,"\#!/bin/bash")
        "call append(line("."), "\################################################")
        "call append(line(".")+1, "\# File Name: ".expand("%:t"))
        "call append(line(".")+2, "\# Description:")
        "call append(line(".")+3, "\# Author: ".authorName)
        "call append(line(".")+4, "\# Mail: ".mailAddr)
        "call append(line(".")+5, "\# Created_Time: ".strftime("%Y-%m-%d %H:%M:%S"))
        "call append(line(".")+6, "\# Last modified: ".strftime("%Y-%m-%d %H:%M:%S"))
        "call append(line(".")+7, "\################################################")
        "call append(line(".")+8, "")
    "如果文件类型为.py文件
    elseif &filetype == 'python'
        call setline(1,"\#!/usr/bin/env python")
        call append(line("."),  "# -*- coding: utf-8 -*-")
        call append(line(".")+1, "")
    " java file
    elseif &filetype == 'java'
        call setline(1,"public class ".expand('%:r')."{")
        call append(line("."),"")
        call append(line(".")+1,"}")
    "php文件固定格式
    elseif &filetype == 'php'
        call setline(1,"<?php")
        call append(line("."),"")
        call append(line(".")+1,"?>")
    elseif &filetype == 'c'
        call setline(1,"#include <stdio.h>")
        call append(line("."), "#include <stdint.h>")
        call append(line(".")+1, "#include <stdlib.h>")
        call append(line(".")+2, "#include <string.h>")
        call append(line(".")+3, "")
    "其他类型文件
    else
        "call setline(1, "/**")
        "call append(line("."),  " * @file: ".expand("%:t"))
        "call append(line(".")+1, " * @brief:")
        "call append(line(".")+2, " * @author: ".authorName)
        "call append(line(".")+3, " * @version: ".versionString)
        "call append(line(".")+4, " * @date: ".strftime("%Y-%m-%d %H:%M:%S"))
        "call append(line(".")+5, " * @modified: ".strftime("%Y-%m-%d %H:%M:%S"))
        "call append(line(".")+6, " */")
        "call append(line(".")+7, "")
    endif

    "c文件自动包含stdio头文件
    "if &filetype == 'c'
    "    call append(line(".")+8, "#include <stdio.h>")
    "    call append(line(".")+9, "#include <stdint.h>")
    "    call append(line(".")+10, "#include <stdlib.h>")
    "    call append(line(".")+11, "#include <string.h>")
    "    call append(line(".")+12, "")
    "endif
    "头文件自动添加框架
    if expand("%:e") == 'h'
        let sourcefilename=expand("%:t")
        let definename=substitute(sourcefilename,' ','','g')
        let definename=substitute(definename,'\.','_','g')
        let definename = toupper(definename)

        call setline(1, '#ifndef __'.definename."__")
        call append(line('.'), '#define __'.definename."__")
        call append(line('.')+1, '')
        call append(line('.')+2, '#ifdef __cplusplus')
        call append(line('.')+3, 'extern "C" {')
        call append(line('.')+4, '#endif')
        call append(line('.')+5, '')
        call append(line('.')+6, '#ifdef __cplusplus')
        call append(line('.')+7, '}')
        call append(line('.')+8, '#endif')
        call append(line('.')+9, '')
        call append(line('.')+10, '#endif')
        exe "normal 15G"
    endif
endfunc

"新建文件后，自动定位到文件末尾
autocmd BufNewFile * normal G

""实现上面函数中的，Last modified功能
"autocmd BufWrite,BufWritePre,FileWritePre  *.sh,*.[ch],*.cpp,*.java   ks|call LastModified()|'s
func LastModified()
    if line("$") > 20
        let l = 20
    else
        let l = line("$")
    endif
    let res = search("@modified","w")
    if res
        exe "1,".l."g/@modified: /s/@modified: .*/@modified:".
                \strftime(" %Y-%m-%d %H:%M:%S" ) . "/e"
    endif
    let res = search("# Last modified","w")
    if res
        exe "1,".l."g/# Last modified: /s/# Last modified: .*/# Last modified:".
                \strftime(" %Y-%m-%d %H:%M:%S" ) . "/e"
    endif
endfunc
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

" 键盘设置
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" tagslist开启关闭快捷键
map <F2> :TagbarToggle<CR>
"NERDTree开启关闭快捷键
map <C-j> :NERDTreeToggle<CR>
"C，C++ 按F5编译运行
map <leader>c :call CompileRunGcc()<CR>
func CompileRunGcc()
	exec "w"
	if &filetype == 'c'
		exec "!gcc % -o %<"
		exec "! ./%<"
	elseif &filetype == 'cpp'
		exec "!g++ % -o %<"
		exec "! ./%<"
	elseif &filetype == 'sh'
		:!bash ./%
    elseif &filetype == 'python'
        exec "!time python3 %"
    elseif &filetype == 'php'
        exec "!time php %"
    elseif &filetype == 'go'
        exec "!time go run %"
	endif
endfunc
" 删除行尾空格
nmap <leader><space> :%s/\s\+$//g<CR>
" 开关行号
nmap <C-\>n :set nu!<CR>
nmap <C-\>y :set nu<CR>
" 开关list
nmap <C-l> :set list<CR>
nmap <C-l>l :set nolist<CR>
" 删除空行
nmap <leader>d :g/^\s*$/d<CR>
" 开关列线
nmap <C-s>c : set cursorcolumn<CR>
nmap <C-s>cf : set nocursorcolumn<CR>
nmap <C-s>r : set cursorline<CR>
nmap <C-s>rf : set nocursorline<CR>
" man 手册
nmap m :!man 3 <C-R>=expand("<cword>")<CR><CR>
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

"自动补全设置
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" TEX中常用大括号
"autocmd BufEnter *.tex inoremap { {}<ESC>i
"autocmd BufEnter *.md inoremap { {}<ESC>i
"autocmd BufEnter *.py inoremap { {}<ESC>i
"autocmd BufEnter *.vue inoremap { {}<ESC>i
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

"cscope设置
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
"快捷键 cscope
"s：查找C代码符号
"g：查找本定义
"d：查找本函数调用的函数
"c：查找调用本函数的函数
"t：查找本字符串
"e：查找本egrep模式
"f：查找本文件
"i：查找包含本文件的文件

if has("cscope")
	set csprg=/usr/bin/cscope
	set csto=0
	set cst
	set nocsverb
	if filereadable("cscope.out")
		cs add $PWD/cscope.out
	else
		let cscope_file=findfile("cscope.out",".;")
		let cscope_pre=matchstr(cscope_file,".*/")
		if !empty(cscope_file)&&filereadable(cscope_file)
			exe "cs add" cscope_file  cscope_pre
		endif
	endif
endif

nmap <C-\>s :cs find s <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>g :cs find g <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>c :cs find c <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>t :cs find t <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>e :cs find e <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>f :cs find f <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>i :cs find i <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>d :cs find d <C-R>=expand("<cword>")<CR><CR>
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

"tagbar设置
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
let g:tagbar_type_vimwiki = {
        \ 'ctagstype' : 'markdown',
        \ 'kinds' : [
                \ 'h:headings',
        \ ],
    \ 'sort' : 0
\ }

"fnc
autocmd BufEnter *.fnc, set filetype=fnc
let g:tagbar_type_fnc = {
        \ 'ctagstype' : 'fnc',
        \ 'kinds' : [
                \ 'c:states',
                \ 'f:functions',
                \ 's:switchs',
                \ 'v:variables',
                \ 'd:dirs',
        \ ],
    \ 'sort' : 0
\ }

"dis objdump -D
autocmd BufEnter *.dis, set filetype=dis
let g:tagbar_type_dis = {
        \ 'ctagstype' : 'dis',
        \ 'kinds' : [
                \ 's:sections',
                \ 'c:contents',
        \ ],
    \ 'sort' : 0
\ }

""设置tagbar使用的ctags的插件,必须要设置对
let g:tagbar_ctags_bin='/usr/bin/ctags'
"设置tagbar的窗口宽度
let g:tagbar_width=30
"设置tagbar的窗口显示的位置,为左边
let g:tagbar_left=1
"打开文件自动 打开tagbar
autocmd BufReadPost *.cpp,*.c,*.h,*.hpp,*.cc,*.cxx call tagbar#autoopen()
autocmd VimEnter * nested :TagbarOpen
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

" clang format
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
let g:yapf_style = "pep8"
autocmd FileType python map <C-K> :Yapf<cr>
autocmd FileType c map <C-K> :%!clang-format<cr>
autocmd FileType cpp map <C-K> :%!clang-format<cr>
autocmd FileType css map <C-K> :%!css-beautify<cr>
autocmd FileType xhtml,html,aspvbs map <C-K> :%!html-beautify<cr>
autocmd FileType js,javascript map <C-K> :%!js-beautify<cr>
autocmd FileType json map <C-K> :%!python -m json.tool<cr>
autocmd FileType xml map <C-K> :%!xmllint --format --recover - 2>/dev/null<cr>
autocmd FileType go map <C-K> :%!gofmt<cr>
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

" doxygen toolkit
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
let g:DoxygenToolkit_briefTag_pre = "@brief"
let g:DoxygenToolkit_paramTag_pre ="@param "
let g:DoxygenToolkit_authorTag = "@author: "
let g:DoxygenToolkit_returnTag = "@return"
let g:DoxygenToolkit_versionTag = "@version: "
let g:DoxygenToolkit_fileTag = "@file: "

let g:DoxygenToolkit_authorName = "yang.zisong"
let g:DoxygenToolkit_versionString = "1.0"

" 函数注释, 光标在函数名上按下快捷键
nmap <leader>f :Dox<CR>
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

" make
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
autocmd FileType c let &makeprg = 'if [ -f Makefile ]; then make; else gcc -Wall -Werror % -lpthread;fi'
autocmd FileType cpp let &makeprg = 'if [ -f Makefile ]; then make; else g++ -Wall -Werror % -lpthread;fi'

if has("autocmd")
    au BufReadPost * if line("'\"") > 1 && line("'\"") <= line("$") | exe "normal! g'\"" | endif
endif

filetype plugin on
autocmd FileType php set omnifunc=phpcomplete#CompletePHP

" phpfmt
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" A standard type: PEAR, PHPCS, PSR1, PSR2, Squiz and Zend
let g:phpfmt_standard = 'PSR2'

let g:phpfmt_autosave = 0

autocmd FileType php map <C-K> :PhpFmt<cr>
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
map <leader>t :!open -a Typora %<CR>

au FileType xml setlocal equalprg=xmllint\ --format\ --recover\ -\ 2>/dev/null
