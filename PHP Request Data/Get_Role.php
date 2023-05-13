<?php
    include "connection.php";

    $card_id = $_GET['CardID'];

    $query = "SELECT role.Role FROM Role JOIN card ON (card.UserID = Role.UserID) WHERE card.CardID ='$card_id' LIMIT 1;";
    
    $result = mysqli_query($connect, $query);

    if(mysqli_num_rows($result) > 0){
        while ($data = mysqli_fetch_array($result)) {
            echo $data['Role'];
        }
    }
    else{
        echo "UNAVAILABLE";
    }
    mysqli_close($connect);
?>