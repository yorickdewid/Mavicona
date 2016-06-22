<?php

use Ecoli\Router;
use Ecoli\Config;

class Ecoli //extends Core
{
	/**
	 * Append settings to config.
	 *
	 * @return Array
	 */
	public function getConfig()
	{
		$config = Config::getInstance();

		return [
			'name' => 'Ecoli',
			'description' => 'Central package manager',

			'database' => [
				'type' => 'pgsql',
				'host' => $config->getConfigOption('DB_HOST'),
				'name' => $config->getConfigOption('DB_NAME'),
				'username' => $config->getConfigOption('DB_USER'),
				'password' => $config->getConfigOption('DB_PASSWD'),
			]
		];
	}

	/**
	 * Register routes.
	 *
	 * @return void
	 */
	public function registerRoutes()
	{
		Router::get('/', 'MainController::getIndex');
		Router::get('repository/main/index/json', 'ApiController::getMainRepository');
		Router::get('api/v1/repository', 'ApiController::getMainRepository');
		Router::post('api/v1/package/new', 'ApiController::doNewPackage');
	}

}