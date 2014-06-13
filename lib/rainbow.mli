module type OrderedType = sig
  type t

  val compare : t -> t -> int
end

module Poney (Ord : OrderedType) : sig
  type key = Ord.t
  type 'a t

  val empty : unit -> 'a t
  val is_empty : 'a t -> bool
  val singleton : key -> 'a -> 'a t
  val mem : key -> 'a t -> bool
  val add : key -> 'a -> 'a t -> 'a t
  val iter : (key -> 'a -> unit) -> 'a t -> unit
end

module Unicorn (Ord : OrderedType) : sig
  type key = Ord.t
  type 'a t

  val create : unit -> 'a t
  val clear : 'a t -> unit
  val add : 'a t -> key -> 'a -> unit
  val remove : 'a t -> key -> unit
  val find : 'a t -> key -> 'a
  val mem : 'a t -> key -> bool
  val iter : (key -> 'a -> unit) -> 'a t -> unit
  val fold : (key -> 'a -> 'b -> 'b) -> 'a t -> 'b -> 'b
end

val dump : 'a -> unit
