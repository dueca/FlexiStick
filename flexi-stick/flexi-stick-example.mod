;; -*-scheme-*-
;; This is an example section for your dueca.mod file.
                    ;; create at normal priority
                    (make-module 'flexi-stick "" sim-priority
                                 ;; timing of the reading/writing
				 'set-timing sim-timing
				 ;; select an SDL joystick, give short name (tiger)
                                 'add-device "tiger:0"

				 ;; stick axis values are already converted
                                 ;; to +/- 1
                                 ;; convert throttle to 0 .. 1, by function
                                 ;; y = 0.5 + 0.5 u, input u is axis 2
				 'create-poly "throttle" "tiger.a[2]"
				 'poly-params 0.5 0.5
				 ;; let the hat 0 up & down control a counter
                                 ;; d[0] is down hat, u[0] is up hat
				 'create-counter "iflaps"
                                 "tiger.d[0]" "tiger.u[0]"
                                 ;; counter from 0 to 3, starting at 0
				 'counter-params 0 3 0
				 ;; convert the flap counter to flap values
				 'create-steps "flaps" "iflaps"
				 'steps-params 0 0  1 5  2 15  3 35

				 ;; channel written
                                 'add-channel "main"           ; var name
                                 "FlexiStickTest://ph-simple"  ; channel full name
                                 "FlexiStickTest"              ; data class
                                 "an example"                  ; entry label
				 
				 ;; link calculated & stick values to channel

                                 ;; button 0 from joystick directly to valb
                                 'add-link "main.valb" "tiger.b[0]"

                                 ;; scaled x to first element in vecd
                                 'add-link "main.vecd[0]" "stickx"

                                 ;; scaled y to second element
                                 'add-link "main.vecd[1]" "sticky"

                                 ;; throttle to valf
				 'add-link "main.valf" "throttle"

                                 ;; flap counter to vals
				 'add-link "main.vals" "iflaps"

                                 ;; and flap value to both vali and vald
				 'add-link "main.vali" "flaps"
				 'add-link "main.vald" "flaps"
                                 )
