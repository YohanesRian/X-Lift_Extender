<?php
    include "connection.php";

    $card_id = $_GET['CardID'];

    $query = "SELECT user.Role FROM user JOIN card ON (card.UserID = user.UserID) WHERE card.CardID ='$card_id' LIMIT 1;";
    
    $result = mysqli_query($connect, $query);

    if(mysqli_num_rows($result) > 0){
        while ($data = mysqli_fetch_array($result)) {
            if(strcasecmp($data['Role'], "Staff") == 0){
                echo $data['Role'];
            }
            else{
                $query = "SELECT DISTINCT room.Floor FROM schedule_access JOIN card ON (schedule_access.UserID = card.UserID) JOIN room ON (schedule_access.RoomID = room.RoomID) WHERE ((schedule_access.AccessIn <= CURRENT_TIMESTAMP AND schedule_access.AccessOut >= CURRENT_TIMESTAMP) OR (schedule_access.AccessIn - INTERVAL 1 HOUR >= CURRENT_TIMESTAMP)) AND card.CardID = '$card_id' LIMIT 1;";
    
                $result = mysqli_query($connect, $query);
                if(mysqli_num_rows($result) > 0){
                    while ($data = mysqli_fetch_array($result)) {
                        echo $data['Floor'];
                    }
                }
                else{
                    echo "NO";
                }
            }
        }
    }
    else{
        echo "NO";
    }
    mysqli_close($connect);
?>