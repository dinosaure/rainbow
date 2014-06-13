module type OrderedType = sig
  type t

  val compare : t -> t -> int
end

module Poney (Ord : OrderedType) = struct
  type key = Ord.t
  type 'a t

  external empty' : (key -> key -> int) -> 'a t
    = "poney_empty"
  external is_empty : 'a t -> bool
    = "poney_is_empty"
  external singleton' : (key -> key -> int) -> key -> 'a -> 'a t
    = "poney_singleton"
  external mem : key -> 'a t -> bool
    = "poney_mem"
  external add : key -> 'a -> 'a t -> 'a t
    = "poney_add"
  external iter : (key -> 'a -> unit) -> 'a t -> unit
    = "poney_mliter"

  let empty () = empty' Ord.compare
  let singleton k v = singleton' Ord.compare k v
end

module Unicorn (Ord : OrderedType) = struct
  type key = Ord.t
  type 'a t

  external create' : (key -> key -> int) -> 'a t
    = "unicorn_create"
  external clear : 'a t -> unit
    = "unicorn_clear"
  external add : 'a t -> key -> 'a -> unit
    = "unicorn_add"
  external remove : 'a t -> key -> unit
    = "unicorn_delete"
  external find : 'a t -> key -> 'a
    = "unicorn_find"
  external mem : 'a t -> key -> bool
    = "unicorn_mem"
  external iter : (key -> 'a -> unit) -> 'a t -> unit
    = "unicorn_mliter"
  external fold : (key -> 'a -> 'b -> 'b) -> 'a t -> 'b -> 'b
    = "unicorn_fold"

  let create () = create' Ord.compare
end

external dump : 'a -> unit = "rainbow_dump"
