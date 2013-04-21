;;; -*- Mode: Lisp; Syntax: Common-Lisp; Package: LANGBAND -*-

#|

DESC: player.lisp - code for the character object
Copyright (c) 2000 - Stig Erik Sand�

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

|#

(in-package :langband)


(defstruct (player (:conc-name player.))
  (name "Foo")
   (class nil)
   (race nil)
   (sex nil)
   
   (base-stats nil) ;; "this is the base stats"
   (curbase-stats nil) ;; "this is the current (possibly drained) base stats"
   (modbase-stats nil) ;; "this is the modified base stats (base + race + class + eq)"
   (active-stats nil) ;; "this is the current active stat-value (curbase + race + class + eq)"

   (loc-x nil)
   (loc-y nil)
   
   (view-x nil)  ;; wx
   (view-y nil)  ;; wy
   
   (depth     0)
   (max-depth nil)
   (level     1)
   (max-level 1)

   (xp-table  nil)
   (max-xp    0)
   (cur-xp    0)
   
   (hp-table  nil)
   (max-hp    0)
   (cur-hp    0)

   (max-mana  0)
   (cur-mana  0)

   
   (leaving-p nil)

   (energy      nil)
   (energy-use  nil)
   (speed       nil)

   (base-ac      0)
   (ac-bonus     0)
   (light-radius 0)
   
   (gold        0)
   (infravision 0)

   (dead-p      nil)

   (equipment   nil)
   ;; quick variable to equipment.backpack.content
   (inventory   nil)

   (skills      nil)

   ;; the map as seen from the player.
   (map         nil) 
   
   )

;; hack, remove later
(defun player.eq (pl-obj)
  (player.equipment pl-obj))

(defun (setf player.eq) (val pl-obj)
  (setf (player.equipment pl-obj) val))

;; move this to variants later
(defclass skills ()
  ((saving-throw :accessor skills.saving-throw  :initform 0)
   (stealth      :accessor skills.stealth       :initform 0)
   (fighting     :accessor skills.fighting      :initform 0)
   (shooting     :accessor skills.shooting      :initform 0)
   (disarming    :accessor skills.disarming     :initform 0)
   (device       :accessor skills.device        :initform 0)
   (perception   :accessor skills.perception    :initform 0)
   (searching    :accessor skills.searching     :initform 0))
  (:documentation "Various skills.."))

(defun make-skills ()
  "Returns a skills object."
  (make-instance 'skills))

(defun get-race-name (player)
  "Returns a string with the name of the race."
  (race.name (player.race player)))

(defun get-class-name (player)
  "Returns a string with the name of the class."
  (class.name (player.class player)))

(defun get-sex-name (player)
  "Returns a string with the name of the sex."
  (cadr (assoc (player.sex player) +sexes+)))


(defun create-player-obj ()
  "Creates and returns a PLAYER object."
  
  (let ((t-p (make-player)))
    (setf (player.base-stats t-p) #1A(0 0 0 0 0 0))
    (setf (player.curbase-stats t-p) #1A(0 0 0 0 0 0))
    (setf (player.modbase-stats t-p) #1A(0 0 0 0 0 0))
    (setf (player.active-stats t-p) #1A(0 0 0 0 0 0))

    (setf (player.skills t-p) (make-skills))
    (setf (player.eq t-p) (make-equipment-slots))

    (setf (player.hp-table t-p) (make-array +max-level+ :initial-element nil))
    (setf (player.xp-table t-p) (make-array +max-level+ :initial-element nil))
    
    (flet ((make-and-assign-backpack! (id)
	     (let ((back-obj (create-aobj-from-id id))
		   (eq-slots (player.eq t-p)))
	       (warn "eq-slots ~a" eq-slots)
	       (item-table-add! eq-slots back-obj 'eq.backpack)
	       (setf (player.inventory t-p) back-obj))))
      
      (let ((backpack-val (game-parameter-value :initial-backpack)))
	(case backpack-val
	  (:backpack (make-and-assign-backpack! :backpack))
	  (otherwise
	   (warn "No initial known backpack-setting, assuming :backpack")
	   (make-and-assign-backpack! :backpack)))))

    ;; hack
    (setf (player.light-radius t-p) 3)
    
    t-p))
 
#||
(defmethod print-object ((inst l-player) stream)
  (print-unreadable-object
   (inst stream :identity t)
   (format stream "~:(~S~) [~A ~A ~A]" (class-name (class-of inst))
	   (player.name inst)
	   (player.race inst)
	   (player.class inst)))
  inst)
||#

(defun get-stat-bonus (player stat-num)
  "Returns the stat-bonus from race, class and equipment for given stat"
  
  (let ((race-mod (race.stat-changes (player.race player)))
	(class-mod (class.stat-changes (player.class player))))
    
    ;; iterate through equipment
    (+ (svref race-mod stat-num)
       (svref class-mod stat-num))))
    
(defun add-stat-bonus (base amount)
  "Returns a numeric value with base incremented with amount"
  (let ((retval base))
    (if (< amount 0)
	(dotimes (i (abs amount))
	  (cond ((>= retval (+ 18 10))
		 (decf retval 10))
		((> retval 18) ;; hackish
		 (setq retval 18))
		((> retval 3) ;; minimum
		 (decf retval 1))
		(t
		 ;; too low to care
		 )))
	;; positive amount
	(dotimes (i amount)
	  (if (< retval 18)
	      (incf retval)
	      (incf retval 10))))
    retval))
		

(defun calculate-stat! (player num)
  "modifies appropriate arrays.."
  
  (let ((base-stat (svref (player.base-stats player) num))
	(cur-stat (svref (player.curbase-stats player) num))
	(bonus (get-stat-bonus player num)))

    (setf (svref (player.modbase-stats player) num) (add-stat-bonus base-stat bonus))
    (setf (svref (player.active-stats player) num) (add-stat-bonus cur-stat bonus)))
  
  (values))

(defun update-player! (player)
  "modifies player object appropriately"

  (let ((race (player.race player)))
  
    (dotimes (i +stat-length+)
      (calculate-stat! player i))


    ;; hackish, change later
    (let ((race-ab (race.abilities race)))
      (dolist (i race-ab)
;;	(Warn "checking ~a" i)
	(when (consp i)
	  (case (car i)
	    ('<infravision> (setf (player.infravision player) (cadr i)))
	    ('<resist> ;; handle later
	     )
	    (otherwise
	     (warn "Unhandled racial ability ~a" (car i)))))))

    ;; reset some key variables
    (setf (player.base-ac player) 0
	  (player.ac-bonus player) 0)
	  
     
    ;; let us skim through items and update variables
    (let ((slots (player.eq player)))
      (item-table-iterate! slots
			   #'(lambda (table key obj)
			       (when obj
				 (let* ((kind (aobj.kind obj))
					(gval (object.game-values kind)))
				   (when gval
				     (incf (player.base-ac player) (gval.base-ac gval))
				     (incf (player.ac-bonus player) (gval.ac-bonus gval))))))
      ))

    
    (bit-flag-add! *redraw* +print-basic+)
    
    player))


(defun gain-level! (player)
  "lets the player gain a level.. woah!  must be updated later"

  (let* ((the-level (player.level player))
	 (hp-table (player.hp-table player))
	 (next-hp (aref hp-table the-level)))

    ;; we have been to this level earlier..
    (unless next-hp
      (let* ((the-class (player.class player))
	     (the-race (player.race player))
	     (hit-dice (+ (class.hit-dice the-class)
			  (race.hit-dice the-race))))
	(setq next-hp (random hit-dice))
	(setf (aref hp-table the-level) next-hp)))

    (incf (player.max-hp player) next-hp)
    (incf (player.level player))

    (when (< (player.max-level player) (player.level player))
      (setf (player.max-level player)  (player.level player)))
	     
    
    ))



(defun increase-xp! (player amount)
  "increases xp for the player. update later."

  (incf (player.cur-xp player) amount)
  (incf (player.max-xp player) amount)

  (loop
   (let* ((cur-level (player.level player))
	  (next-limit (aref (player.xp-table player) cur-level)))
  
     (if (> (player.cur-xp player) next-limit)
	 (gain-level! player)
	 (return-from increase-xp! nil)))
  
   ))


;; map related stuff:
(defun cave-info-from-map (player x y)
  (aref (player.map player) x y))



(defun (setf cave-info-from-map) (val player x y)
;;  (warn "Setting ~a ~a to ~a" x y val)
  (setf (aref (player.map player) x y) val)
  ;; more
  )
