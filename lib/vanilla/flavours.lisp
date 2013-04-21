;;; -*- Mode: Lisp; Syntax: Common-Lisp; Package: LANGBAND -*-

#|

vanilla/flavours.lisp - flavours for vanilla variant
Copyright (c) 2000 - Stig Erik Sand�

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

----

Contains all flavouring relevant to the Vanilla variant

|#

(in-package :langband)

(define-flavour-type '<potion>)
(define-flavour-type '<ring>)
(define-flavour-type '<amulet>)
(define-flavour-type '<staff>)
(define-flavour-type '<wand>)
(define-flavour-type '<rod>)
(define-flavour-type '<mushroom>)
;; scrolls are defined at the end of this file


;; the potion flavours

(define-basic-flavour '<potion> "azure" +term-l-blue+)
(define-basic-flavour '<potion> "black" +term-l-dark+)
(define-basic-flavour '<potion> "blue" +term-blue+)
(define-basic-flavour '<potion> "blue speckled" +term-blue+)
(define-basic-flavour '<potion> "brown" +term-umber+)
(define-basic-flavour '<potion> "brown speckled" +term-umber+)
(define-basic-flavour '<potion> "bubbling" +term-red+)
(define-basic-flavour '<potion> "chartreuse" +term-l-green+)
(define-basic-flavour '<potion> "clear" +term-white+)
(define-basic-flavour '<potion> "clotted red" +term-red+)
(define-basic-flavour '<potion> "cloudy" +term-white+)
(define-basic-flavour '<potion> "coagulated crimson" +term-red+)
(define-basic-flavour '<potion> "copper speckled" +term-l-umber+)
(define-basic-flavour '<potion> "crimson" +term-red+)
(define-basic-flavour '<potion> "cyan" +term-l-blue+)
(define-basic-flavour '<potion> "dark blue" +term-blue+)
(define-basic-flavour '<potion> "dark green" +term-green+)
(define-basic-flavour '<potion> "dark red" +term-red+)
(define-basic-flavour '<potion> "gloopy green" +term-green+)
(define-basic-flavour '<potion> "gold" +term-yellow+)
(define-basic-flavour '<potion> "gold speckled" +term-yellow+)
(define-basic-flavour '<potion> "green" +term-green+)
(define-basic-flavour '<potion> "green speckled" +term-green+)
(define-basic-flavour '<potion> "grey" +term-slate+)
(define-basic-flavour '<potion> "grey speckled" +term-slate+)
(define-basic-flavour '<potion> "hazy" +term-l-white+)
(define-basic-flavour '<potion> "icky green" +term-green+)
(define-basic-flavour '<potion> "indigo" +term-l-blue+)
(define-basic-flavour '<potion> "light blue" +term-l-blue+)
(define-basic-flavour '<potion> "light brown" +term-l-umber+)
(define-basic-flavour '<potion> "light green" +term-l-green+)
(define-basic-flavour '<potion> "magenta" +term-red+)
(define-basic-flavour '<potion> "metallic blue" +term-blue+)
(define-basic-flavour '<potion> "metallic green" +term-green+)
(define-basic-flavour '<potion> "metallic purple" +term-violet+)
(define-basic-flavour '<potion> "metallic red" +term-red+)
(define-basic-flavour '<potion> "misty" +term-l-white+)
(define-basic-flavour '<potion> "oily yellow" +term-yellow+)
(define-basic-flavour '<potion> "orange" +term-orange+)
(define-basic-flavour '<potion> "orange speckled" +term-orange+)
(define-basic-flavour '<potion> "pink" +term-l-red+)
(define-basic-flavour '<potion> "pink speckled" +term-l-red+)
(define-basic-flavour '<potion> "puce" +term-violet+)
(define-basic-flavour '<potion> "pungent" +term-l-red+)
(define-basic-flavour '<potion> "purple" +term-violet+)
(define-basic-flavour '<potion> "purple speckled" +term-violet+)
(define-basic-flavour '<potion> "red" +term-red+)
(define-basic-flavour '<potion> "red speckled" +term-red+)
(define-basic-flavour '<potion> "shimmering" +term-red+)
(define-basic-flavour '<potion> "silver speckled" +term-l-white+)
(define-basic-flavour '<potion> "smoky" +term-l-dark+)
(define-basic-flavour '<potion> "tangerine" +term-orange+)
(define-basic-flavour '<potion> "vermilion" +term-red+)
(define-basic-flavour '<potion> "violet" +term-violet+)
(define-basic-flavour '<potion> "violet speckled" +term-violet+)
(define-basic-flavour '<potion> "viscous pink" +term-l-red+)
(define-basic-flavour '<potion> "white" +term-white+)
(define-basic-flavour '<potion> "yellow" +term-yellow+)
(define-basic-flavour '<potion> "yellow speckled" +term-yellow+)


;; the mushrooms
(define-basic-flavour '<mushroom> "blue" +term-blue+)	
(define-basic-flavour '<mushroom> "black" +term-l-dark+)	
(define-basic-flavour '<mushroom> "black spotted" +term-l-dark+)	
(define-basic-flavour '<mushroom> "brown" +term-umber+)	
(define-basic-flavour '<mushroom> "dark blue" +term-blue+)	
(define-basic-flavour '<mushroom> "dark green" +term-green+)	
(define-basic-flavour '<mushroom> "dark red" +term-red+)	
(define-basic-flavour '<mushroom> "yellow" +term-yellow+)	
(define-basic-flavour '<mushroom> "furry" +term-l-white+)	
(define-basic-flavour '<mushroom> "green" +term-green+)	
(define-basic-flavour '<mushroom> "grey" +term-slate+)	
(define-basic-flavour '<mushroom> "light blue" +term-l-blue+)	
(define-basic-flavour '<mushroom> "light green" +term-l-green+)	
(define-basic-flavour '<mushroom> "red" +term-red+)
(define-basic-flavour '<mushroom> "slimy" +term-slate+)	
(define-basic-flavour '<mushroom> "tan" +term-l-umber+)
(define-basic-flavour '<mushroom> "violet" +term-violet+)	
(define-basic-flavour '<mushroom> "white" +term-white+)	
(define-basic-flavour '<mushroom> "white spotted" +term-white+)	
(define-basic-flavour '<mushroom> "wrinkled" +term-umber+)	

;; the wand flavours

(define-basic-flavour '<wand> "aluminum" +term-l-blue+)
(define-basic-flavour '<wand> "aluminum-plated" +term-l-blue+)
(define-basic-flavour '<wand> "brass" +term-l-umber+)
(define-basic-flavour '<wand> "bronze" +term-l-umber+)
(define-basic-flavour '<wand> "cast iron" +term-l-dark+)
(define-basic-flavour '<wand> "chromium" +term-white+)
(define-basic-flavour '<wand> "copper" +term-l-umber+)
(define-basic-flavour '<wand> "copper-plated" +term-l-umber+)
(define-basic-flavour '<wand> "gold" +term-yellow+)
(define-basic-flavour '<wand> "gold-plated" +term-yellow+)
(define-basic-flavour '<wand> "iron" +term-slate+)
(define-basic-flavour '<wand> "ivory" +term-white+)
(define-basic-flavour '<wand> "lead" +term-slate+)
(define-basic-flavour '<wand> "lead-plated" +term-slate+)
(define-basic-flavour '<wand> "magnesium" +term-l-white+)
(define-basic-flavour '<wand> "mithril" +term-l-blue+)
(define-basic-flavour '<wand> "mithril-plated" +term-l-blue+)
(define-basic-flavour '<wand> "molybdenum" +term-l-white+)
(define-basic-flavour '<wand> "nickel" +term-l-umber+)
(define-basic-flavour '<wand> "nickel-plated" +term-l-umber+)
(define-basic-flavour '<wand> "pewter" +term-slate+)
(define-basic-flavour '<wand> "platinum" +term-white+)
(define-basic-flavour '<wand> "rusty" +term-red+)
(define-basic-flavour '<wand> "runed" +term-umber+)
(define-basic-flavour '<wand> "silver" +term-l-white+)
(define-basic-flavour '<wand> "silver-plated" +term-l-white+)
(define-basic-flavour '<wand> "steel" +term-l-white+)
(define-basic-flavour '<wand> "steel-plated" +term-l-white+)
(define-basic-flavour '<wand> "tin" +term-l-white+)
(define-basic-flavour '<wand> "tin-plated" +term-l-white+)
(define-basic-flavour '<wand> "titanium" +term-white+)
(define-basic-flavour '<wand> "tungsten" +term-white+)
(define-basic-flavour '<wand> "zirconium" +term-l-white+)
(define-basic-flavour '<wand> "zinc" +term-l-white+)
(define-basic-flavour '<wand> "zinc-plated" +term-l-white+)	

;; staff flavours

(define-basic-flavour '<staff> "aspen" +term-l-umber+)
(define-basic-flavour '<staff> "balsa" +term-l-umber+)
(define-basic-flavour '<staff> "banyan" +term-l-umber+)
(define-basic-flavour '<staff> "birch" +term-l-umber+)
(define-basic-flavour '<staff> "cedar" +term-l-umber+)
(define-basic-flavour '<staff> "cottonwood" +term-l-umber+)
(define-basic-flavour '<staff> "cypress" +term-l-umber+)
(define-basic-flavour '<staff> "dogwood" +term-l-umber+)
(define-basic-flavour '<staff> "elm" +term-l-umber+)
(define-basic-flavour '<staff> "eucalyptus" +term-l-umber+)
(define-basic-flavour '<staff> "hemlock" +term-l-umber+)
(define-basic-flavour '<staff> "hickory" +term-l-umber+)
(define-basic-flavour '<staff> "ironwood" +term-umber+)
(define-basic-flavour '<staff> "locust" +term-l-umber+)
(define-basic-flavour '<staff> "mahogany" +term-umber+)
(define-basic-flavour '<staff> "maple" +term-l-umber+)
(define-basic-flavour '<staff> "mulberry" +term-l-umber+)
(define-basic-flavour '<staff> "oak" +term-l-umber+)
(define-basic-flavour '<staff> "pine" +term-l-umber+)
(define-basic-flavour '<staff> "redwood" +term-red+)
(define-basic-flavour '<staff> "rosewood" +term-red+)
(define-basic-flavour '<staff> "spruce" +term-l-umber+)
(define-basic-flavour '<staff> "sycamore" +term-l-umber+)
(define-basic-flavour '<staff> "teak" +term-l-umber+)
(define-basic-flavour '<staff> "walnut" +term-umber+)
(define-basic-flavour '<staff> "mistletoe" +term-green+)
(define-basic-flavour '<staff> "hawthorn" +term-l-umber+)
(define-basic-flavour '<staff> "bamboo" +term-l-umber+)
(define-basic-flavour '<staff> "silver" +term-white+)
(define-basic-flavour '<staff> "runed" +term-umber+)
(define-basic-flavour '<staff> "golden" +term-yellow+)
(define-basic-flavour '<staff> "ashen" +term-slate+)
(define-basic-flavour '<staff> "gnarled" +term-umber+)
(define-basic-flavour '<staff> "ivory" +term-umber+)
(define-basic-flavour '<staff> "willow" +term-umber+)

;; amulet flavours

(define-basic-flavour '<amulet> "agate" +term-l-white+)
(define-basic-flavour '<amulet> "amber" +term-yellow+)
(define-basic-flavour '<amulet> "azure" +term-l-blue+)
(define-basic-flavour '<amulet> "bone" +term-white+)
(define-basic-flavour '<amulet> "brass" +term-l-umber+)
(define-basic-flavour '<amulet> "bronze" +term-l-umber+)
(define-basic-flavour '<amulet> "copper" +term-l-umber+)
(define-basic-flavour '<amulet> "coral" +term-white+)
(define-basic-flavour '<amulet> "crystal" +term-white+)
(define-basic-flavour '<amulet> "driftwood" +term-l-umber+)
(define-basic-flavour '<amulet> "golden" +term-yellow+)
(define-basic-flavour '<amulet> "ivory" +term-white+)
(define-basic-flavour '<amulet> "obsidian" +term-l-dark+)
(define-basic-flavour '<amulet> "pewter" +term-slate+)
(define-basic-flavour '<amulet> "silver" +term-white+)
(define-basic-flavour '<amulet> "tortoise shell" +term-umber+)

;; ring flavours
(define-basic-flavour '<ring> "adamantite" +term-l-green+)
(define-basic-flavour '<ring> "alexandrite"  +term-green+)
(define-basic-flavour '<ring> "amethyst" +term-violet+)
(define-basic-flavour '<ring> "aquamarine" +term-l-blue+)
(define-basic-flavour '<ring> "azurite" +term-l-blue+)
(define-basic-flavour '<ring> "beryl" +term-l-green+)
(define-basic-flavour '<ring> "bloodstone" +term-red+)
(define-basic-flavour '<ring> "bronze" +term-l-umber+)
(define-basic-flavour '<ring> "calcite" +term-white+)
(define-basic-flavour '<ring> "carnelian" +term-red+)
(define-basic-flavour '<ring> "corundum" +term-slate+)
(define-basic-flavour '<ring> "diamond" +term-white+)
(define-basic-flavour '<ring> "emerald" +term-green+)
(define-basic-flavour '<ring> "engagement" +term-yellow+)
(define-basic-flavour '<ring> "fluorite" +term-l-green+)
(define-basic-flavour '<ring> "garnet" +term-red+)
(define-basic-flavour '<ring> "gold" +term-yellow+)
(define-basic-flavour '<ring> "granite" +term-l-white+)
(define-basic-flavour '<ring> "jade" +term-l-green+)
(define-basic-flavour '<ring> "jasper" +term-umber+)
(define-basic-flavour '<ring> "jet" +term-l-dark+)
(define-basic-flavour '<ring> "lapis lazuli" +term-blue+)
(define-basic-flavour '<ring> "malachite" +term-green+)
(define-basic-flavour '<ring> "marble" +term-white+)
(define-basic-flavour '<ring> "mithril" +term-l-blue+)
(define-basic-flavour '<ring> "moonstone" +term-l-white+)
(define-basic-flavour '<ring> "obsidian" +term-l-dark+)
(define-basic-flavour '<ring> "onyx" +term-l-red+)
(define-basic-flavour '<ring> "opal" +term-l-white+)
(define-basic-flavour '<ring> "pearl" +term-white+)
(define-basic-flavour '<ring> "platinum" +term-white+)
(define-basic-flavour '<ring> "quartz" +term-l-white+)
(define-basic-flavour '<ring> "quartzite" +term-l-white+)
(define-basic-flavour '<ring> "rhodonite" +term-l-red+)
(define-basic-flavour '<ring> "ruby" +term-red+)
(define-basic-flavour '<ring> "sapphire" +term-blue+)
(define-basic-flavour '<ring> "silver" +term-l-white+)
(define-basic-flavour '<ring> "tiger eye" +term-yellow+)
(define-basic-flavour '<ring> "topaz" +term-yellow+)
(define-basic-flavour '<ring> "tortoise shell" +term-umber+)
(define-basic-flavour '<ring> "turquoise" +term-l-blue+)
(define-basic-flavour '<ring> "zircon" +term-l-umber+)

;; the rod and wand tables are currently combined

(use-flavour-table '<wand> '<rod>)

(defconstant +scroll-syllables+ #1A(
				    "a" "ab" "ag" "aks" "ala" "an" "ankh" "app"
				    "arg" "arze" "ash" "aus" "ban" "bar" "bat" "bek"
				    "bie" "bin" "bit" "bjor" "blu" "bot" "bu"
				    "byt" "comp" "con" "cos" "cre" "dalf" "dan"
				    "den" "der" "doe" "dok" "eep" "el" "eng" "er" "ere" "erk"
				    "esh" "evs" "fa" "fid" "flit" "for" "fri" "fu" "gan"
				    "gar" "glen" "gop" "gre" "ha" "he" "hyd" "i"
				    "ing" "ion" "ip" "ish" "it" "ite" "iv" "jo"
				    "kho" "kli" "klis" "la" "lech" "man" "mar"
				    "me" "mi" "mic" "mik" "mon" "mung" "mur" "nag" "nej"
				    "nelg" "nep" "ner" "nes" "nis" "nih" "nin" "o"
				    "od" "ood" "org" "orn" "ox" "oxy" "pay" "pet"
				    "ple" "plu" "po" "pot" "prok" "re" "rea" "rhov"
				    "ri" "ro" "rog" "rok" "rol" "sa" "san" "sat"
				    "see" "sef" "seh" "shu" "ski" "sna" "sne" "snik"
				    "sno" "so" "sol" "sri" "sta" "sun" "ta" "tab"
				    "tem" "ther" "ti" "tox" "trol" "tue" "turs" "u"
				    "ulk" "um" "un" "uni" "ur" "val" "viv" "vly"
				    "vom" "wah" "wed" "werg" "wex" "whon" "wun" "x"
				    "yerg" "yp" "zun" "tri" "blaa"))

(defvar *van-used-scroll-names* (make-hash-table :test #'equal)
  "a table with already created names of scrolls")

(defun van-make-scroll-name ()
  "Returns a string with the name of a scroll"
  (let ((scroll-name "")
	(syl-len (length +scroll-syllables+)))
    (loop
     (let ((syl-num (random 2)))
       (setq scroll-name (concatenate 'string
				      scroll-name
				      (svref +scroll-syllables+ (random syl-len))
				      ;; possible second syllable
				      (if (= syl-num 1)
					  (svref +scroll-syllables+ (random syl-len))
					  "")
				      " "))
       ;; if long enough, return
       (when (< 15 (length scroll-name))
	 (return-from van-make-scroll-name (string-right-trim '(#\Space #\Tab #\Newline) scroll-name)))))

    "<failure in scr name-gen>"))

	 

(defun van-generate-scroll-flavour (object)
  "returns the flavour for the given object"
  
  (declare (ignore object))
  
  ;; make a name for the scroll
  (loop named naming-loop
	for name = (van-make-scroll-name)
	for hash-val = (gethash name *van-used-scroll-names*)
	do
	(unless hash-val
	  (setf (gethash name *van-used-scroll-names*) t)
	  (return-from van-generate-scroll-flavour (list name +term-white+)))))

;; time to add the scroll flavour type
(define-flavour-type '<scroll> #'van-generate-scroll-flavour)

