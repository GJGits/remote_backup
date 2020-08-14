<?php
include("sidebars.php");
session_start();

global $class_id;
if(isset($_GET['class'])) {
    $class_id = $_GET['class'];
}

?>

<!doctype html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <meta name="description" content="">
    <meta name="author" content="Mark Otto, Jacob Thornton, and Bootstrap contributors">
    <meta name="generator" content="Jekyll v3.8.5">
    <title>Teacher Record Marks</title>

    <!-- Bootstrap core CSS -->
    <link href="css/bootstrap.min.css" rel="stylesheet" >


    <style>
        .bd-placeholder-img {
            font-size: 1.125rem;
            text-anchor: middle;
            -webkit-user-select: none;
            -moz-user-select: none;
            -ms-user-select: none;
            user-select: none;
        }

        @media (min-width: 768px) {
            .bd-placeholder-img-lg {
                font-size: 3.5rem;
            }
        }

        table {
            font-family: arial, sans-serif;
            border-collapse: collapse;
            width: 100%;
        }

        td, th {
            border: 1px solid #dddddd;
            text-align: left;
            padding: 8px;
        }

        tr:nth-child(even) {
            background-color: #dddddd;
        }
    </style>
    <!-- Custom styles for this template -->
    <link href="css/dashboard.css" rel="stylesheet">
    <link href="css/style.css" rel="stylesheet">

</head>
<body>

<script>

    function reloadPage_classSelected(){
        var classElement = document.getElementById("inlineFormCustomSelect");
        var className = classElement.options[classElement.selectedIndex].id;
        window.location.replace("teacher_page_marks.php?class=" + className);
    }

    function toggle_laude(i) {
        var mark = parseFloat(document.getElementById("mark_"+i).value);
        if(mark == 10)
            document.getElementById("laude_"+i).disabled = false;
        else{
            document.getElementById("laude_"+i).checked = false;
            document.getElementById("laude_"+i).disabled = true;
        }
    }

</script>


<nav class="navbar navbar-expand-md navbar-dark bg-dark sticky-top">
    <a class="navbar-brand" href="#">Teacher Account</a>
    <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarNavDropdown" aria-controls="navbarNavDropdown" aria-expanded="false" aria-label="Toggle navigation">
        <span class="navbar-toggler-icon"></span>
    </button>
    <div class="collapse navbar-collapse" id="navbarNavDropdown">

        <ul class="navbar-nav ml-auto">
            <li class="nav-item align-left">
                <button type="submit" class="btn btn-danger" onclick="location.href='logout_post.php';">Sign out</button>
            </li>
        </ul>
    </div>
</nav>

<div class="container-fluid">
    <div class="row">
        <nav class="col-md-2 d-md-block bg-light sidebar">
            <div class="small_screen" >
                <ul class="nav flex-column">
                    <li class="nav-item">
                        <div class="user-info">
                            <div class="image"><img src="photos/student.jpg" alt="User"></a></div>
                            <!-- Per modificare la immagine del teacher rendendola un link mettere <a href="profile.html"> tra div e img-->
                            <div class="detail">
                                <h4><?php echo $_SESSION["name_u"]." ".$_SESSION["surname_u"]; ?></h4>
                            </div>
                        </div>
                    </li>
                    <?php
                    teacher_print_sidebar("true");
                    ?>

                </ul>

            </div>
        </nav>

        <main role="main" class="col-md-9 ml-sm-auto col-lg-10 px-4">
            <div class="pt-3 pb-2 mb-3 border-bottom">
                <h2>Record Marks</h2>


                <form action = "insert_mark_record.php" method = "post">
                    <div class="form-row align-items-center">
                        <!-- SELECT CLASS -->
                        <div class="col-auto my-1">
                            <label class="mr-sm-2" for="inlineFormCustomSelect">Select Classroom</label>
                            <select class="custom-select mr-sm-2" name = "class" id="class" onchange="reloadPage_classSelected()">
                                <?php
                                if($class_id == "")
                                    echo "<option selected hidden></option>";
                                $conn = mysqli_connect("localhost", "root", "", "esrms");

                                $sql = "SELECT DISTINCT c.name, c.cid
																		FROM class c, bridge_class_teachers bct
																		WHERE c.cid = bct.cid
																		AND bct.ssn_t = '" . $_SESSION["user_ssn"] . "'";
                                //die("A" . $sql);

                                if (!$result = mysqli_query($conn, $sql)) {
                                    $msg = "Errore nell’inserimento del post, riprovare";
                                }
                                $temp = mysqli_num_rows($result);

                                while ($row = $result->fetch_assoc()) {
                                    //$class_id = $row['cid'];
                                    echo "<option id=" . $row['cid'];
                                    if ($row['cid'] == $class_id)
                                        echo " selected";
                                    echo ">" . $row['name'] . "</option>";
                                }
                                echo "<input type='hidden' name='cid' value='".$class_id."'>";
                                ?>
                            </select>
                        </div>

                        <!-- SELECT SUBJECT -->
                        <div class="col-auto my-1">
                            <label class="mr-sm-2" for="inlineFormCustomSelect">Select Subject</label>
                            <select class="custom-select mr-sm-2" name = "subject" id="inlineFormCustomSelect">
                                <?php
                                if($class_id!="") {
                                    $conn = mysqli_connect("localhost", "root", "", "esrms");

                                    $sql = "SELECT subject
                                        FROM bridge_class_teachers
                                        WHERE ssn_t = '" . $_SESSION['user_ssn'] . "'
                                        AND cid = '" . $class_id . "'";
                                    //die($sql);
                                    //AND cid = '<script>document.getElementById("classSelected").innerHTML</script> ';
                                    /*
                                                                            FROM students s, bridge_class_students bcs
                                                                            WHERE s.ssn_s = bcs.ssn_s
                                                                            AND bcs.cid = 'C1'";*/
                                    //die("A" . $sql);

                                    if (!$result = mysqli_query($conn, $sql)) {
                                        $msg = "Errore nell’inserimento del post, riprovare";
                                    }
                                    $temp = mysqli_num_rows($result);

                                    while ($row = $result->fetch_assoc()) {
                                        echo "<option>" . $row['subject'] . "</option>";
                                    }
                                    //echo "<option>Test Subject</option>";*/
                                }
                                ?>
                            </select>
                        </div>

                    </div>



                    <div class="form-group row align-items-center">
                        <label for="example-date-input" class="col-2 col-form-label">Date</label>
                        <div class="col-4">
                            <input class="form-control" type="date" name = "date" value=<?php print(date("Y-m-d")); ?> id="example-date-input">
                        </div>
                        <h7 id="error_date" style="color:red">
                            <?php
                            if(isset($_GET['error_future_date']))
                                echo $_GET['error_future_date'];
                            if(isset($_GET['error_expired_time_window']))
                                echo $_GET['error_expired_time_window'];
                            if(isset($_GET['error_not_school_hour']))
                                echo $_GET['error_not_school_hour'];
                            ?>
                        </h7>
                        <label for="example-time-input" class="col-2 col-form-label">Time</label>
                        <div class="col-4">
                            <input class="form-control" type="time" name = "time" value=<?php print(date("H:i")); ?> id="example-time-input" max=>
                        </div>

                    </div>



                    <div class="form-group align-items-center">
                        <label for="exampleFormControlTextarea1">Description lecture</label>
                        <textarea class="form-control" id="exampleFormControlTextarea1" name = "description" rows="1"></textarea>
                    </div>


                    <div><input type="hidden" name="ssn_t" value=<?php print $_SESSION["user_ssn"]; ?>></div>

                    <!-- LIST STUDENT -->
                    <div class="form-group align-items-center">
                        <?php
                        if($class_id != "") {
                            $conn = mysqli_connect("localhost", "root", "", "esrms");

                            $sql = "SELECT s.name, s.surname, s.ssn
                                                            FROM students s, bridge_class_students bcs
                                                            WHERE s.ssn = bcs.ssn_s
                                                            AND bcs.cid = '" . $class_id . "'
                                                            ORDER BY s.surname";

                            if (!$result = mysqli_query($conn, $sql)) {
                                $msg = "Errore nell’inserimento del post, riprovare";
                                echo $msg;
                            } else {
                                $temp = mysqli_num_rows($result);

                                echo "<table>";
                                echo "<tr>
                                                                <th>Surname</th>
                                                                <th>Name</th>
                                                                <th>Mark</th>
                                                                <th>Laude</th>
                                                            </tr>";
                                $i = 0;
                                while ($row = $result->fetch_assoc()) {
                                    $ssn = $row['ssn'];
                                    echo "<tr>
                                                                        <td>" . $row['surname'] . "</td>
                                                                        <td>" . $row['name'] . "</td>
                                                                        <td><input name='stud_".$ssn ."' id='mark_" . $ssn . "' type='number' step='0.25' min='0' max='10' onchange='toggle_laude(\"".$ssn."\")'>
                                                                        <h7 id='error_mark_".$ssn."' style=\"color:red\">";
                                    if(isset($_GET["error_wrong_mark_".$ssn]))
                                        echo $_GET["error_wrong_mark_".$ssn];
                                    echo "</h7>
                                                                        </td>
                                                                        <td><input name='laude_".$ssn ."' id='laude_" . $ssn . "' type='checkbox' disabled></td>
                                                                    </tr>";
                                    $i++;
                                }
                                echo "</table>";
                            }
                        }
                        ?>

                        <h7 style="color:red">
                            <?php
                            if(isset($_GET['error_no_marks']))
                                echo $_GET['error_no_marks'];
                            ?>
                        </h7>
                    </div>


                    <div class="form-group row " >
                        <?php
                        if(isset($_GET['class']))
                            echo "<button type=\"submit\" class=\"btn btn-primary align-right\"  >Submit</button>";
                        ?>
                    </div>

                </form>



            </div>

        </main>
    </div>
</div>
<script src="https://code.jquery.com/jquery-3.4.1.min.js"></script>
<script src="https://code.jquery.com/jquery-3.4.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
<script>window.jQuery || document.write('<script src="/docs/4.3/assets/js/vendor/jquery-slim.min.js"><\/script>')</script><script src="js/bootstrap.bundle.min.js" ></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/feather-icons/4.9.0/feather.min.js"></script>
<script src="js/dashboard.js"></script></body>
</html>
