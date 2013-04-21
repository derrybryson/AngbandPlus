#|

DESC: tools/ffi-build.lisp - batch lisp-code for building FFI-declarations

|#

(in-package :cl-user)

(setq ext:*gc-verbose* nil
      *compile-print* nil)

(pushnew :building-ffi-defs *features*)

;;(print *features*)

(load "../package.lisp")

(in-package :langband-ffi)

(load "ffi-gen")
(load "../ffi/ffi-defs")

(generate-for-type :cmucl "../ffi/ffi-cmu.lisp")
(generate-for-type :acl   "../ffi/ffi-acl.lisp")
(generate-for-type :clisp "../ffi/ffi-clisp.lisp")

#+cmu
(when ext:*batch-mode*
  (cl-user::quit))
