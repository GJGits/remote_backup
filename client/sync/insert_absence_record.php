<?php

    session_start();
    $conn = mysqli_connect("localhost","root", "", "esrms");
    if (mysqli_connect_errno()) {
        echo "Connessione fallita: ".
            mysqli_connect_error();
        exit();
    }

    /*
    foreach($_POST as $key => $value){
        echo "<tr>";
        echo "<td>";
        echo $key . " </td>" . $value . "</td><br>";
        echo "</tr>";
        echo "</tr>";
    }
    */

    /* Fetch data from $_POST */
    $ssn_t = $_POST['ssn_t'];     // Or $_POST['ssn_t']
    $cid = $_POST['cid'];
    $class = $_POST['class'];
    $subject = $_POST['subject'];
    $date = $_POST['date'];
    $time = $_POST['time'];

    /* Check of valid date and time (e.g. today is 24/11/2019) */
    $now = time();
    $now_date = date('d-m-Y', $now);
    $now_date = strtotime($now_date);               // 24/11/2019, 00:00

    $input_date = strtotime($date);                 // [input_date], 00:00

    $input_time = strtotime($time);
    $input_time -= $now_date;                       // 01/01/1970, [input_time]

    $input_date_time = $input_date + $input_time;   // [input_date], [input_time]

    if($input_date_time > $now){                    // Date/time in future
        header("location:teacher_page_absences.php?class=".$cid."&error_future_date=Error: future date and/or time!");
        die();
    }

    $school_openingHour = strtotime("01/01/1970, 09:00:00");
    $school_closingHour = strtotime("01/01/1970, 16:00:00");
    if($input_time<$school_openingHour || $input_time>$school_closingHour){    // Not school hour
        header("location:teacher_page_absences.php?class=".$cid."&error_not_school_hour=Error: school classes last from 08:00 to 15:00!");
        die();
    }

    $date_diff = $now - $input_date_time;
    $date_diff = round($date_diff/(60*60*24));
    if($date_diff > 30){                            // More than a month passed since present date
        header("location:teacher_page_absences.php?class=".$cid."&error_expired_time_window=Error: more than a month has passed since this date!");
        die();
    }

    if(strlen($time)!=5){
        $time = "0".$time[0].":00";
    }
    else{
        $time = $time[0].$time[1].":00";

    }

    /* Fetch keys */
    $keys = array_keys($_POST);

    /* Fill student, absence and description arrays */
    $students = array();
    $absences = array();
    $descriptions = array();
    /*$wrong_marks = 0;
    $wrong_marks_get = '';*/
    $at_least_one_absence = 0;
    for($i=0; $i<count($keys); $i++){
        if(substr($keys[$i], 0, 12) == "absenceForm_"){
            $ssn_s = substr($keys[$i], 12);
            $absence = $_POST[$keys[$i]];
            if($absence == 'Absent'){
                $description = "Absent";
                array_push($students, $ssn_s);
                array_push($absences, $absence);
                array_push($descriptions, $description);
            }
            if($absence == 'Partial'){
                $description = $_POST[$keys[$i+1]];
                if($description=='')
                    $description = "Partially present";
                array_push($students, $ssn_s);
                array_push($absences, $absence);
                array_push($descriptions, $description);
            }
            if($absence!="-")
                $at_least_one_absence++;
        }
    }

    if($at_least_one_absence==0){
        header("location:teacher_page_absences.php?class=".$cid."&error_no_absences=Error: no absence has been written!");
        die();
    }

    $conn = mysqli_connect("localhost","root", "", "esrms");
    if (mysqli_connect_errno()) {
        echo "Connessione fallita: ". mysqli_connect_error();
        exit();
    }

    /* Select the last abs_id */
    $sql = "SELECT *
                    FROM absences_presences";
    if(!$result =mysqli_query($conn,$sql)) {
        $msg = "Errore nell’inserimento del post, riprovare";
    }
    $temp = mysqli_num_rows($result);
    $temp++;

    for($i=0; count($students)>0 && $i<count($students); $i++){
        /* Insert marks into DB */
        switch($absences[$i]){
            case "Partial":
                $state = 'P';
                break;

            case "Absent":
                $state = 'A';
                break;
        }

        $sql = "INSERT INTO absences_presences(abs_id, ssn_s, date, time, state, description, ssn_t, subject, cid)
                        VALUES ('".$temp."','".$students[$i]."','".$date."','".$time."','".$state."','".$descriptions[$i]."','".$ssn_t."','".$subject."','".$cid."')";
        //die($sql);
        if(!$result =mysqli_query($conn,$sql)) {
            die("Errore nell’inserimento del post, riprovare");
        }
        $temp++;



    }
    header("location:teacher_page.php?successful_op=The absences have been successfully inserted");

?>
