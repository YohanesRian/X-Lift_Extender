<?php
    include "connection.php";

    $card_id = $_GET['CardID'];

    $query = "SELECT DISTINCT room.floor FROM schedule_access JOIN card ON (schedule_access.UserID = card.UserID) JOIN room ON (schedule_access.RoomID = room.RoomID) WHERE ((schedule_access.AccessIn <= CURRENT_TIMESTAMP AND schedule_access.AccessOut >= CURRENT_TIMESTAMP) OR (schedule_access.AccessIn - INTERVAL 1 HOUR <= CURRENT_TIMESTAMP)) AND card.CardID = '$card_id' LIMIT 1;";
    
    $result = mysqli_query($connect, $query);

    if(mysqli_num_rows($result) > 0){
        while ($data = mysqli_fetch_array($result)) {
            echo $data['floor'];
        }
    }
    else{
        echo "NONE";
    }
    mysqli_close($connect);
?>