function parse_inputs() {
  let array_of_inputs = new Array;

  let my_cards_table = document.getElementById("my_cards");
  let my_cards_rows = my_cards_table.querySelectorAll("tr");
  my_cards_rows.forEach(my_cards_row => {
    let fields = my_cards_row.querySelectorAll("input");
    let array_of_fields = Array.from(fields);
    let array_of_row_values = array_of_fields.map(field => field.value);
    if (array_of_row_values.length > 0) {
      array_of_inputs.push(array_of_row_values);
    }
  });

  let opponents_cards_table = document.getElementById("opponents_cards");
  let opponents_cards_rows = opponents_cards_table.querySelectorAll("tr");
  opponents_cards_rows.forEach(opponents_cards_row => {
    let fields = opponents_cards_row.querySelectorAll("input");
    let array_of_fields = Array.from(fields);
    let array_of_row_values = array_of_fields.map(field => field.value);
    if (array_of_row_values.length > 0) {
      array_of_inputs.push(array_of_row_values);
    }
  });

  let simple_states_table = document.getElementById("simple_states");
  let state_ord = simple_states_table.querySelector("#ord").value;
  let state_qty = simple_states_table.querySelector("#qty").value;
  let state_seq = simple_states_table.querySelector("#seq").checked ? 1 : 0;
  let state_rev = simple_states_table.querySelector("#rev").checked ? 1 : 0;
  let state_b11 = simple_states_table.querySelector("#b11").checked ? 1 : 0;
  let state_lock = simple_states_table.querySelector("#lock").checked ? 1 : 0;
  let state_onset = simple_states_table.querySelector("#onset").checked ? 1 : 0;
  let state_joker = simple_states_table.querySelector("#joker").checked ? 1 : 0;
  /* 2次元配列にするために，要素1つのものもムリヤリ配列にする． */
  array_of_inputs.push([state_ord]);
  array_of_inputs.push([state_seq]);
  array_of_inputs.push([state_qty]);
  array_of_inputs.push([state_rev]);
  array_of_inputs.push([state_b11]);
  array_of_inputs.push([state_lock]);
  array_of_inputs.push([state_onset]);

  let array_of_state_player_qty = new Array;
  let state_player_qty_table = document.getElementById("state_player_qty");
  let state_player_qty_inputs = state_player_qty_table.querySelectorAll("input")
  state_player_qty_inputs.forEach(state_player_qty_input => {
    array_of_state_player_qty.push(state_player_qty_input.value);
  });

  let array_of_state_seat = new Array;
  let state_seat_table = document.getElementById("state_seat");
  let state_seat_inputs = state_seat_table.querySelectorAll("input")
  state_seat_inputs.forEach(state_seat_input => {
    array_of_state_seat.push(state_seat_input.value);
  });

  let array_of_state_player_rank = new Array;
  let state_player_rank_table = document.getElementById("state_player_rank");
  let state_player_rank_inputs = state_player_rank_table.querySelectorAll("input")
  state_player_rank_inputs.forEach(state_player_rank_input => {
    array_of_state_player_rank.push(state_player_rank_input.value);
  });

  let array_of_state_suit = new Array;
  let state_suit_table = document.getElementById("state_suit");
  let state_suit_inputs = state_suit_table.querySelectorAll("input")
  state_suit_inputs.forEach(state_suit_input => {
    array_of_state_suit.push(state_suit_input.checked ? 1 : 0);
  });

  array_of_inputs.push(array_of_state_suit);
  array_of_inputs.push(array_of_state_player_qty);
  array_of_inputs.push(array_of_state_player_rank);
  array_of_inputs.push(array_of_state_seat);
  array_of_inputs.push([state_joker]);
  
  return array_of_inputs;
}

function array_to_tsv(arr_2D) {
  tsv_str = "";

  tsv_str = arr_2D.map(arr => {
    return arr.join("\t");
  }).join("\n");

  return tsv_str;
}

function save_inputs_as_tsv() {
  array_of_inputs = parse_inputs();
  tsv_str = array_to_tsv(array_of_inputs);

  download_link = document.getElementById("download_link");
  blob = new Blob([tsv_str], {type: "text/plain"});
  download_link = document.createElement("a");
  download_link.href = URL.createObjectURL(blob);
  download_link.download = "simulation.tsv";
  download_link.click();
}