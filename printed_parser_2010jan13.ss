
;;;;;;;;;  extra  ;;;;;;;;;;

(define (analyze-atomic-string-token E)
  (cond ((eq? E "lambda") 'lambda) 
        ((eq? E "if") 'if)
        ((eq? E "cond") 'cond)
        ((eq? E "or") 'or)
        ((eq? E "and") 'and)
        ((eq? E "#f") #f)
        ((eq? E "#t") #t)
        ((string->number E) (string->number E))
        (else E)
        ))

(define (empty? L)
  (eq? L '()))

(define (whitespace? A)
  (or (eq? A #\space)
      (eq? A #\newline)
      (eq? A #\tab)))

;;;;;;;;;;;;;;;;;;;

(define read-next-token
  (lambda (accum L parencount init )
    (if (empty? L )
        (list (reverse accum ) '() )
        (if init
            (if (eq? (car L ) #\( )
                (read-next-token (cons (car L ) accum ) (cdr L ) (+ parencount 1 ) #f )
                (if (eq? (car L ) #\) )
                    (read-next-token accum (cdr L ) parencount #t ) 
                    (if (whitespace? (car L ) )
                        (read-next-token accum (cdr L ) parencount #t )
                        (read-next-token (cons (car L ) accum ) (cdr L ) parencount #f ) ) )
                )
            (if (eq? parencount 0 )
                (if (or (whitespace? (car L ) ) (eq? #\) (car L ) ) (eq? #\( (car L ) ) )
                    (list (reverse accum ) L ) 
                    (read-next-token (cons (car L ) accum ) (cdr L ) parencount #f ) )
                (if (eq? (car L ) #\( )
                    (read-next-token (cons (car L ) accum ) (cdr L ) (+ parencount 1 ) #f )
                    (if (eq? (car L ) #\) )
                        (read-next-token (cons (car L ) accum ) (cdr L ) (+ parencount -1 ) #f )
                        (read-next-token (cons (car L ) accum ) (cdr L ) parencount #f )
                        ) ) ) ) ) ))


(define iterated-read-next-token
  (lambda (L )
    (if (empty? L ) 
        '()
        ( (lambda (E ) (if (not (empty? (car E ) ) )
                           (cons (parse (car E ) ) (iterated-read-next-token (cadr E ) ) )
                           '() ) ) 
          (read-next-token '() L 0 #t ) ) ) ))

(define parse
  (lambda (L )
    (if (empty? L ) 
        '()
        (if (eq? (car L ) #\( ) 
            (iterated-read-next-token (cdr L ) ) 
            (analyze-atomic-string-token (list->string L )) ) ) ))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(parse (string->list "(lambda () 1)"))