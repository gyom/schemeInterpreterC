
(define map
  (lambda (f L)
    (if (empty? L)
        empty
        (cons (f (car L)) (map f (cdr L))))))

(define h
  (lambda ()
    (define A
      (call/cc (lambda (throw)
                 (define f
                   (lambda (x)
                     (if (eq? x 0)
                         (call/cc (lambda (continue) (throw continue)))
                         (* 7 x))))
                 (map f (list 1 0 2 3 0)))))
    (if (continuation? A)
        (A 'lupi)
        A)))




