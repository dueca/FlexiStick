;; -*-scheme-*-
;; this is an example dueca.mod file, for you to start out with and adapt
;; according to your needs. Note that you need a dueca.mod file only for the
;; node with number 0

(define guidev #t)
(define sticky 1)

;; NOTE: for new guile (from openSUSE 12.1 onwards), you may only once use
;; (define ...). For modifying an already defined value, used (set! ...)

;; in general, it is a good idea to clearly document your set up
;; this is an excellent place.
;; node set-up
(define ecs-node 0)    ; dutmms1, send order 3
;(define aux-node 1)   ; dutmms3, send order 1
;(define pfd-node 2)   ; dutmms5, send order 2
;(define cl-node 3)    ; dutmms4, send order 0

;; priority set-up
; normal nodes: 0 administration
;               1 simulation, unpackers
;               2 communication
;               3 ticker

; administration priority. Run the interface and logging here
(define admin-priority (make-priority-spec 0 0))

; priority of simulation, just above adiminstration
(define sim-priority (make-priority-spec 1 0))

; nodes with a different priority scheme
; control loading node has 0, 1 and 2 as above and furthermore
;               3 stick priority
;               4 ticker priority
; priority of the stick. Higher than prio of communication
(define stick-priority (make-priority-spec 3 0))

; timing set-up
; timing of the stick calculations. Assuming 500 usec ticks, this gives 2000 Hz
(define stick-timing (make-time-spec 0 1))

; this is normally 20, giving 100 Hz timing
(define sim-timing (make-time-spec 0 200))

;; for now, display on 50 Hz
(define display-timing (make-time-spec 0 40))

;; log a bit more economical, 25 Hz
(define log-timing (make-time-spec 0 80))

;;; the modules needed for dueca itself
(dueca-list
  (make-entity "dueca"
	       (if (equal? 0 this-node-id)
		   (list
		    (make-module 'dusime "" admin-priority)
		    (make-module 'dueca-view "" admin-priority)
		    (make-module 'activity-view "" admin-priority)
		    (make-module 'timing-view "" admin-priority)
		    (make-module 'log-view "" admin-priority)
		    (make-module 'channel-view "" admin-priority)
		    )
		 (list)
		 )
	       )
  )

;;; the modules for your application(s)
(define citation
  (make-entity "ph-simple"
	       (if (and (equal? ecs-node this-node-id) (not guidev))
		   (list
		    (make-module 'flexi-stick "" sim-priority
				 'set-timing sim-timing
				 ;; select an SDL joystick, give short name
                                 ;; (tiger), link to dev 0
                                 'add-device "tiger:0"
				 ;; values are already coverted to -1 .. 1
				 'create-poly "stickx" "tiger.a[0]"
				 'poly-params 0.0 1.0
				 'create-poly "sticky" "tiger.a[1]"
				 'poly-params 0.0 1.0
				 'create-poly "throttle" "tiger.a[2]"
				 'poly-params 0.5 0.5
				 ;; let the hat up & down control a counter
				 'create-counter "iflaps" "tiger.d[0]"
                                 "tiger.u[0]"
				 'counter-params 0 3 0
				 ;; convert the iflap counter to flap values
				 'create-steps "flaps" "iflaps"
				 'steps-params 0 0  1 5  2 15  3 35
                                 ;; for kicks, add a summation of
                                 ;; iflaps and the left hat
                                 'create-weighted "wsum"
                                 "2.0*iflaps" "-1.3*tiger.l[0]"

				 ;; channel written
                                 'add-channel "main"           ; var name
                                 "FlexiStickTest://ph-simple"  ; channel full name
                                 "FlexiStickTest"              ; data class
                                 "first try"                   ; entry label
				 "event"

				 ;; link calculated & stick values to channel
                                 'add-link "main.valb" "tiger.b[0]"
                                 'add-link "main.vecd[0]" "stickx"
                                 'add-link "main.vecd[1]" "sticky"
				 'add-link "main.valf" "tiger.a[2]"
				 'add-link "main.vals" "tiger.b[0]"
				 'add-link "main.vali" "tiger.h[0]"
				 'add-link "main.vald" "wsum"

				 'add-channel "second"
				 "MediumJoystick://ph-simple"
				 "MediumJoystick"
				 "verify"
				 "event"

				 'add-link "second.axis[0]" "stickx"
				 'add-link "second.axis[1]" "sticky"
				 'add-link "second.axis[2]" "tiger.a[2]"
				 'add-link "second.button[0]" "tiger.b[0]"
				 'add-link "second.hat[0]" "tiger.h[0]"

                                 )
                   (make-module 'stick-view "" admin-priority
                                'set-timing sim-timing)
                   )
		 )
	       (if (and (equal? ecs-node this-node-id) guidev)
		   (list
		    (make-module 'flexi-stick "" sim-priority
				 'set-timing sim-timing

                                 'add-virtual "tiger"
                                 'add-virtual-slider 20 20 20 100 5 sticky
                                 'add-virtual-slider-2d 100 100 190 190 6 sticky
                                 'add-virtual-button 50 10 5 sticky
                                 'add-virtual-hat 50 50 15 sticky

                                 ;; channel written
                                 'add-channel "main"           ; var name
                                 "FlexiStickTest://ph-simple"  ; channel full name
                                 "FlexiStickTest"              ; data class
                                 "first try"                   ; entry label
				 ;; link calculated & stick values to channel
                                 'add-link "main.valb" "tiger.b[0]"
                                 'add-link "main.vecd[0]" "tiger.a[0]"
                                 'add-link "main.vecd[1]" "tiger.a[1]"
				 'add-link "main.valf" "tiger.a[2]"
				 'add-link "main.vals" "tiger.b[0]"
				 'add-link "main.vali" "tiger.h[0]"
				 'add-link "main.vald" "tiger.u[0]")

                    (make-module 'stick-view "" admin-priority
                                 'set-timing sim-timing)
                    )
                   )


               ; an empty list; at least one list should be supplied
               ; for nodes that have no modules as argument
               (list)
               )
  )
