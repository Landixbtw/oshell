;; -*- lexical-binding: t; -*-

(TeX-add-style-hook
 "main"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("article" "12pt" "english")))
   (TeX-add-to-alist 'LaTeX-provided-package-options
                     '(("fontenc" "T1") ("inputenc" "utf8") ("graphicx" "") ("float" "") ("babel" "english") ("a4wide" "") ("xcolor" "") ("listings" "") ("tikz" "") ("enumitem" "") ("url" "") ("csquotes" "") ("verbatim" "") ("newfloat" "") ("todonotes" "") ("indentfirst" "") ("svg" "") ("caption" "") ("fvextra" "") ("xspace" "") ("lipsum" "") ("newcomputermodern" "") ("hyphenat" "htt") ("minted" "newfloat") ("xparse" "") ("biblatex" "backend=biber" "style=ieee" "language=english") ("hyperref" "hidelinks") ("cleveref" "")))
   (add-to-list 'LaTeX-verbatim-environments-local "ccode")
   (add-to-list 'LaTeX-verbatim-environments-local "lstlisting")
   (add-to-list 'LaTeX-verbatim-environments-local "Verbatim")
   (add-to-list 'LaTeX-verbatim-environments-local "Verbatim*")
   (add-to-list 'LaTeX-verbatim-environments-local "BVerbatim")
   (add-to-list 'LaTeX-verbatim-environments-local "BVerbatim*")
   (add-to-list 'LaTeX-verbatim-environments-local "LVerbatim")
   (add-to-list 'LaTeX-verbatim-environments-local "LVerbatim*")
   (add-to-list 'LaTeX-verbatim-environments-local "VerbatimOut")
   (add-to-list 'LaTeX-verbatim-environments-local "SaveVerbatim")
   (add-to-list 'LaTeX-verbatim-environments-local "VerbEnv")
   (add-to-list 'LaTeX-verbatim-environments-local "VerbatimWrite")
   (add-to-list 'LaTeX-verbatim-environments-local "minted")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "
ewminted{c}")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "c")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "EscVerb*")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "EscVerb")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "Verb*")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "Verb")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "lstinline")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "href")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "hyperimage")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "hyperbaseurl")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "nolinkurl")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "url")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "path")
   (add-to-list 'LaTeX-verbatim-macros-with-delims-local "
ewminted{c}")
   (add-to-list 'LaTeX-verbatim-macros-with-delims-local "c")
   (add-to-list 'LaTeX-verbatim-macros-with-delims-local "lstinline")
   (add-to-list 'LaTeX-verbatim-macros-with-delims-local "path")
   (add-to-list 'LaTeX-verbatim-macros-with-delims-local "Verb")
   (add-to-list 'LaTeX-verbatim-macros-with-delims-local "Verb*")
   (TeX-run-style-hooks
    "latex2e"
    "article"
    "art12"
    "fontenc"
    "inputenc"
    "graphicx"
    "float"
    "babel"
    "a4wide"
    "xcolor"
    "listings"
    "tikz"
    "enumitem"
    "url"
    "csquotes"
    "verbatim"
    "newfloat"
    "todonotes"
    "indentfirst"
    "svg"
    "caption"
    "fvextra"
    "xspace"
    "lipsum"
    "newcomputermodern"
    "hyphenat"
    "minted"
    "xparse"
    "biblatex"
    "hyperref"
    "cleveref")
   (TeX-add-symbols
    '("inlinecode" TeX-arg-verb-delim-or-brace)
    '("mycomment" 1)
    '("funcname" 1)
    '("usubsubsection" 1)
    '("usubsection" 1)
    '("usection" 1)
    '("mintedlabel" 1)
    "projectname")
   (LaTeX-add-labels
    "line:#1"
    "sec:planning"
    "fig:mainLoopFlowchart"
    "fig:executeCommandFlowchart"
    "mnt:WhatShows"
    "mnt:cd"
    "sec:piping"
    "fig:pipeVis"
    "fig:multipipe"
    "fig:memoryLayout"
    "mnt:clear"
    "sec:inputRedirection"
    "fig:inputRedirection"
    "sec:outputRedirection"
    "fig:outputRedirection"
    "mnt:envVar"
    "mnt:oshellLoop"
    "mnt:removeQuotes"
    "mnt:changeDir"
    "mnt:kill"
    "mnt:inputRedirection"
    "mnt:outputRedirection"
    "mnt:findOperator"
    "mnt:bqString"
    "mnt:pipeRedirection")
   (LaTeX-add-environments
    "code")
   (LaTeX-add-bibliographies
    "oshell")
   (LaTeX-add-xcolor-definecolors
    "pblue"
    "pgreen"
    "pred"
    "pyellow"
    "pgrey"
    "codebg"
    "pblack"
    "templeos-bg"
    "templeos-fg"
    "templeos-cyan"
    "templeos-green"
    "templeos-yellow"
    "templeos-red"
    "templeos-gray")
   (LaTeX-add-listings-lstdefinestyles
    "inlinecommandcore")
   (LaTeX-add-minted-newmints
    '("" "c" "ed")))
 :latex)

