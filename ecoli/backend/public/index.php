<?php

/**
 * Ecoli backend - Central package repository
 *
 * @package  Ecoli
 * @author   Yorick de Wid <yorick17@outlook.com>
 */

define("FW_VERSION", "Version 0.1");

/*
|--------------------------------------------------------------------------
| Load classes
|--------------------------------------------------------------------------
*/

try {

require __DIR__ . '/../app/Exception.php';
require __DIR__ . '/../app/Application.php';
require __DIR__ . '/../app/SingletonContract.php';
require __DIR__ . '/../app/Config.php';
require __DIR__ . '/../app/Router.php';
require __DIR__ . '/../app/Database.php';
require __DIR__ . '/../app/Log.php';
require __DIR__ . '/../app/Request.php';
require __DIR__ . '/../app/Response.php';
require __DIR__ . '/../Ecoli.php';
require __DIR__ . '/../controller/ApiController.php';
require __DIR__ . '/../controller/MainController.php';

/*
|--------------------------------------------------------------------------
| Create The Application
|--------------------------------------------------------------------------
|
| The first thing we will do is create a new application instance
| which serves as the "glue" for all the components, and is
| the IoC container for the system binding all of the various parts.
|
*/

$app = new Ecoli\Application(Ecoli::class);

$app->singleton([
    Ecoli\Config::class,
    Ecoli\Router::class,
    Ecoli\Database::class,
    Ecoli\Log::class,
]);

/*
|--------------------------------------------------------------------------
| Run The Application
|--------------------------------------------------------------------------
|
| Once we have the application, we can handle the incoming request
| through the application core, and send the associated response back to
| the client's browser.
|
*/

$app->loadCore();

$response = $app->handle(
    Ecoli\Request::capture()
);

$response->send();

$app->terminate();

} catch (\Exception $e) {
	echo "<pre>Uncaught exception: " . $e->getMessage() . "</pre>";
}