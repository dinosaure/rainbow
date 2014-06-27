open Core_bench.Std

module Int = struct
  type t = int

  let compare = compare
end

module E = Rainbow.Unicorn(Int)

let print_list ?(sep=", ") print_data out_ch lst =
  let rec aux = function
    | [] -> ()
    | [ x ] -> print_data out_ch x
    | x :: r ->
      Printf.fprintf out_ch "%a%s"
        print_data x
        sep;
      aux r
  in aux lst

let print_string out_ch = Printf.fprintf out_ch "%s"
let print_int out_ch = Printf.fprintf out_ch "%d"
let print_tuple print_a print_b out_ch (a, b) =
  Printf.fprintf out_ch "(%a, %a)" print_a a print_b b

let rainbow_insert tbl number =
  let rec aux = function
    | 0 ->
      E.add tbl 0 (Random.int 1000);
    | n ->
      E.add tbl n (Random.int 1000);
      aux (n - 1)
  in aux number

let hashtbl_insert tbl number =
  let rec aux = function
    | 0 ->
      Hashtbl.add tbl 0 (Random.int 1000);
    | n ->
      Hashtbl.add tbl n (Random.int 1000);
      aux (n - 1)
  in aux number

let t1 =
  ("Rainbow insert 1000",
   (fun () -> rainbow_insert (E.create ()) 1000))
let t2 =
  ("Hashtbl[false,1] insert 1000",
   (fun () -> hashtbl_insert (Hashtbl.create 1) 1000))
let t3 =
  ("Hashtbl[true,1] insert 1000",
   (fun () -> hashtbl_insert (Hashtbl.create ~random:true 1) 1000))
let t4 =
  ("Hashtbl[false,512] insert 1000",
   (fun () -> hashtbl_insert (Hashtbl.create 512) 1000))
let t5 =
  ("Hashtbl[true,512] insert 1000",
   (fun () -> hashtbl_insert (Hashtbl.create ~random:true 512) 1000))

let benchmark () =
  Core.Command.run
    (Bench.make_command
       (List.map
          (fun (name, test) -> Bench.Test.create ~name test)
          [t1; t2; t3; t4; t5]))

let () = benchmark ()
