module M = Rainbow.Poney(String)
module U = Rainbow.Unicorn(String)

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

let () =
  let a = U.create () in

  U.add a "toto" 1010;
  U.add a "titi" 1111;
  U.add a "tata" 1313;
  U.add a "tutu" 1717;

  U.iter (fun key value -> Printf.fprintf stderr "%s: %d\n%!" key value) a;

  let l1 = U.fold
      (fun key value acc ->
         Gc.compact ();
         (key, value) :: acc)
      a []
  in

  Printf.fprintf stderr "%a\n%!" (print_list (print_tuple print_string print_int)) l1;

  ()
