<!doctype html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <meta name="description" content="">
    <title>Teacher Page</title>
	
	 <!-- Bootstrap core CSS -->
    <link href="css/bootstrap.min.css" rel="stylesheet" >
    

    <!-- Vendor CSS-->
    <link href="vendor/select2/select2.min.css" rel="stylesheet" media="all">
    <link href="vendor/datepicker/daterangepicker.css" rel="stylesheet" media="all">
    
    <!-- Main CSS-->
    <link href="css/add_homework_form.css" rel="stylesheet" media="all">
    
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.0/jquery.min.js"></script>

<?php
 include("sidebars.php");
 session_start();
 
global $class_id;
if(isset($_GET['class'])) {
    $class_id = $_GET['class'];
}

?>

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

<div>

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
  <?php 
	 if(isset($_GET["successful_op"])){
		 echo "<h1 style='color:green'>".$_GET["successful_op"]."</h1>";
	 }
	 ?>  
      <main role="main" class="col-md-9 ml-sm-auto col-lg-10 px-4">
		 <div class="wrapper wrapper--w790">
            <div class="card card-5">
                <div class="card-heading">
                    <h2 class="title">Add new homework</h2>
                </div>
                <div class="card-body">
                    <form action="add_student.php" method="POST" id='add_stud'>
                        <div class="form-row">
                            <div class="name" >Choose a file:*</div>
                            <div class="value">
                                <div class="input-group">
                                    <input type="file" name="myFile" id='myFile' class='btn--red'>
                                </div>
                            </div>
                        </div>
                        <div class="form-row">
                            <div class="name">Description:</div>
                            <div class="value">
                                <div class="input-group">
								 <textarea class="form-control" id="description" name = "description" rows="3"></textarea>
                                </div>
                            </div>
                        </div>
                         <div class="form-row m-b-55">
                            <div class="name">Class:*</div>
                            <div class="value">
                                <div class="row row-refine">
                                    <div class="col-9">
                                        <div class="input-group-desc">
                                          <select class="custom-select mr-sm-2" name = "class" id="option" onchange="reloadPage_classSelected()">
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
                                    </div>
                                </div>
                            </div>
                        </div>
                         <div class="form-row m-b-55">
                            <div class="name">Subject:*</div>
                            <div class="value">
                                <div class="row row-refine">
                                    <div class="col-9">
                                        <div class="input-group-desc">
                                           <select class="custom-select mr-sm-2" name = "subject" id="option">
                                <?php
                                if($class_id!="") {
                                    $conn = mysqli_connect("localhost", "root", "", "esrms");

                                    $sql = "SELECT subject
                                        FROM bridge_class_teachers
                                        WHERE ssn_t = '" . $_SESSION['user_ssn'] . "'
                                        AND cid = '" . $class_id . "'";
                                    if (!$result = mysqli_query($conn, $sql)) {
                                        $msg = "Errore nell’inserimento del post, riprovare";
                                    }
                                    $temp = mysqli_num_rows($result);

                                    while ($row = $result->fetch_assoc()) {
                                        echo "<option>" . $row['subject'] . "</option>";
                                    }
                                }
                                ?>
                            </select>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                         <div class="form-row">
                            <div class="name" >Date:*</div>
                            <div class="value">
                                 <div class="row row-refine">
                                    <div class="col-9">
                                        <div class="input-group-desc">
                                       
                     
                            <input class="form-control" type="date" name = "date" value=<?php print(date("Y-m-d")); ?> id="example-date-input">
                                        </div>
                                    </div>
                                </div>  
                            </div>
                        </div>
                        <div>
			               <h4  class= "message"></h4>
                            <button id="addstudent" class="btn btn--radius-2 btn--red" type="submit">Submit</button>
                            
                        </div>
                    </form>
                </div>
            </div>
        </div>
    
   
     
    </main>



	 
    </main>
  </div>
</div>
<script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
      <script>window.jQuery || document.write('<script src="/docs/4.3/assets/js/vendor/jquery-slim.min.js"><\/script>')</script><script src="js/bootstrap.bundle.min.js" ></script>
        <script src="https://cdnjs.cloudflare.com/ajax/libs/feather-icons/4.9.0/feather.min.js"></script>
        <script src="js/dashboard.js"></script></body>
</html>
