(define (empty? L)
  (eq? L '()))

(define (whitespace? A)
  (or (eq? A #\space)
      (eq? A #\newline)
      (eq? A #\tab)))

(define (read-next-token accum L parencount init)
  (if (empty? L)
      (list (reverse accum) '())
      (let ((added (cons (car L) accum))
            (remaining (cdr L)))
        (if init
            (cond ((eq? (car L) #\() (read-next-token added remaining (+ parencount 1) #f))
                   ; skip lonely closing parentheses that are a side effect of the line *side-effect*
                  ((eq? (car L) #\)) (read-next-token accum remaining parencount #t))
                  ((whitespace? (car L)) (read-next-token accum remaining parencount #t))
                  (#t (read-next-token added remaining parencount #f)))
            (if (eq? parencount 0)
                (if (or (whitespace? (car L)) (eq? #\) (car L)) (eq? #\( (car L)))
                    (list (reverse accum) L)
                    (read-next-token added remaining parencount #f))
                (cond ((eq? (car L) #\( ) (read-next-token added remaining (+ parencount 1) #f))
                      ((eq? (car L) #\) ) (read-next-token added remaining (- parencount 1) #f))
                      (#t (read-next-token added remaining parencount #f))
                      ))))))

(define (analyze-atomic-element E)
  (cond ((eq? (list->string E) "lambda") 'lambda) 
        ((eq? (list->string E) "if") 'if)
        ((eq? (list->string E) "cond") 'cond)
        ((eq? (list->string E) "or") 'or)
        ((eq? (list->string E) "and") 'and)
        ((eq? (list->string E) "#f") #f)
        ((eq? (list->string E) "#t") #t)
        ((string->number (list->string E)) (string->number (list->string E)))
        (else (list->string E))
        ))

(define (iterated-read-next-token L)
  (if (empty? L)
      '()
      (let* ((E (read-next-token '() L 0 #t))
             (token (car E))
             (rest (cadr E)))
        (if (not (empty? token))
            (cons (parse token) (iterated-read-next-token rest))
            '()))))
  
(define (parse L)
  (printf "parsing : ~s\n" (list->string L))
  (cond ((empty? L) '())
        ((eq? (car L) #\( ) (iterated-read-next-token (cdr L))) ; line *side-effect*
        (#t (analyze-atomic-element L))
        (else (being (display L) 'ERROR))
        ))
         
(define (print-by-line L)
  (if (empty? L)
      'done
      (begin (display (car L)) (newline) (print-by-line (cdr L)))))

(define E (string->list "(lambda (x) (+ 1 x))"))
(list->string (car (read-next-token '() E 0 #t)))

(define E1 (string->list "(patate) chien"))
(list->string (car (read-next-token '() E1 0 #t)))

(define E2 (string->list "patate chien"))
(list->string (car (read-next-token '() E2 0 #t)))

(define E3 (string->list "( ( patate con ) ) "))
(list->string (car (read-next-token '() E3 0 #t)))

(define E4 (string->list "( ( patate con ) ))")) ; with one extra closing )
(list->string (car (read-next-token '() E4 0 #t)))

;(print-by-line (map list->string (iterated-read-next-token (string->list "(lambda (x) (+ 1 x)) lupi -123 (LAUL)"))))
(define A (iterated-read-next-token (string->list "(lambda (x) (+ 1 x)) lupi -123 (LAUL)")))
(print-by-line A)
(display A)