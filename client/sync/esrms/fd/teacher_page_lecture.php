<?php
include("sidebars.php");
session_start();
?>

<!doctype html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <meta name="description" content="">
    <meta name="author" content="Mark Otto, Jacob Thornton, and Bootstrap contributors">
    <meta name="generator" content="Jekyll v3.8.5">
    <title>Dashboard Template · Bootstrap</title>



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
    </style>
    <!-- Custom styles for this template -->
    <link href="css/dashboard.css" rel="stylesheet">
		<link href="css/style.css" rel="stylesheet">

</head>
<body>

<script>

    function fill_classSelected(){
        // Get the combo box element
        var classElement = document.getElementById("inlineFormCustomSelect");

        // Get the value selected from the combo box
        var className = classElement.options[classElement.selectedIndex].value;

        // Put that value in the hidden input field of the form for the class name
        document.getElementById("classSelected").value = className;
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
                <h2>Record Hourly lecture</h2>


                <form action = "insert_lesson_record.php" method = "post">
                    <div class="form-row align-items-center">
                        <div class="col-auto my-1">
                            <label class="mr-sm-2" for="inlineFormCustomSelect">Select Classroom</label>
                            <select class="custom-select mr-sm-2" name = "classe" id="inlineFormCustomSelect">
                                <?php
                                $conn = mysqli_connect("localhost","root", "", "esrms");

                                $sql = "SELECT DISTINCT c.name
                        FROM class c, bridge_class_teachers bct
                        WHERE c.cid = bct.cid
                        AND bct.ssn_t = '" .$_SESSION["user_ssn"] . "'";
                                //die("A" . $sql);

                                if(!$result = mysqli_query($conn,$sql)) {
                                    $msg = "Errore nell’inserimento del post, riprovare";
                                }
                                $temp = mysqli_num_rows($result);

                                while($row = $result->fetch_assoc()) {
                                    echo "<option>".$row['name']."</option>";
                                    //echo "<option name='classe' value = '".$row['nome_classe']."'>".$row['nome_classe']."<input  name='classe' value='".$row['nome_classe']."'> </option>"  ;
                                }
                                ?>
                            </select>
                        </div>


                    </div>
			
			
			
				     <div class="form-row align-items-center">
                        <div class="col-auto my-1">
                           Subject: <input type="text" name="subject"><br>     
								
							
                        </div>


                    </div>
			
			
			
			
			
                    <div class="form-group align-items-center">
                        <label for="exampleFormControlTextarea1">Description lecture</label>
                        <textarea class="form-control" id="exampleFormControlTextarea1" name = "description" rows="3"></textarea>
                    </div>

                    <div class="form-group row align-items-center">
                        <label for="example-date-input" class="col-2 col-form-label">Date</label>
                        <div class="col-4">
                            <input class="form-control" type="date" name = "date" value=<?php print(date("Y-m-d")); ?> id="example-date-input">
                        </div>
                    </div>
                    <div class="form-group row">
                        <label for="example-time-input" class="col-2 col-form-label">Start Hour Lesson</label>
                        <div class="col-4">
                            <input class="form-control" type="time" name = "time" value=<?php print(date("H:00")); ?> id="example-time-input">
                        </div>

                    </div>
                    <div><input type="hidden" name="ssn_t" value=<?php print $_SESSION["user_ssn"]; ?>></div>
                    <div class="form-group row " >
                        <button type="submit" class="btn btn-primary align-right"  >Submit</button>
                    </div>
					<div> <input type="hidden" name= "user_ssn" value="<?php echo $_SESSION["user_ssn"];?>"></div>
                </form>



            </div>

        </main>
    </div>
</div>
<script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
<script>window.jQuery || document.write('<script src="/docs/4.3/assets/js/vendor/jquery-slim.min.js"><\/script>')</script><script src="js/bootstrap.bundle.min.js" ></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/feather-icons/4.9.0/feather.min.js"></script>
<script src="js/dashboard.js"></script></body>
</html>
