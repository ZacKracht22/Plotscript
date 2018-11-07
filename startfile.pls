(begin
(define make-point (lambda (x y) (set-property "object-name" "point" (set-property "size" (0) (list x y)))))
(define make-line (lambda (p1 p2) (set-property "object-name" "line" (set-property "thickness" (1) (list p1 p2)))))
(define make-text (lambda (str) (set-property "object-name" "text" (set-property "position" (list 0 0) (set-property "text-scale" 1 (set-property "text-rotation" 0 (str)))))))
)